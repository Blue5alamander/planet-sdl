#include <planet/functional.hpp>
#include <planet/log.hpp>
#include <planet/sdl/audio.hpp>
#include <planet/sdl/init.hpp>
#include <planet/telemetry/counter.hpp>
#include <planet/telemetry/duration.hpp>
#include <planet/telemetry/load.hpp>
#include <planet/telemetry/rate.hpp>

#include <felspar/exceptions/logic_error.hpp>


using namespace std::literals;
using namespace planet::audio::literals;


static_assert(
        std::atomic<planet::audio::sample_clock>::is_always_lock_free,
        "Playback-head atomic must be lock-free on the real-time audio thread");


planet::sdl::audio_output::audio_output(configuration &config)
: config{config} {
    last_master_mul = config.master_volume.multiplier();
    reconnect(config.audio_device_name);
}


planet::sdl::audio_output::~audio_output() {
    /**
     * Close the device first so the callback can never touch an attached mixer
     * again; the mixers (owned elsewhere) are destroyed after this returns.
     */
    reset();
}


void planet::sdl::audio_output::reset() {
    if (device > 0) { SDL_CloseAudioDevice(std::exchange(device, 0)); }
}


void planet::sdl::audio_output::reserve_mixers(std::size_t const count) {
    mixers.reserve(count);
    last_underruns.resize(count, 0);
}


void planet::sdl::audio_output::attach(audio::mixer &m) {
    std::scoped_lock lock{attach_mtx};
    if (mixers.size() == mixers.capacity()) {
        /**
         * `push_back` here would reallocate and move the buffer the audio
         * callback reads lock-free on the real-time thread. Every mixer-taking
         * constructor calls `reserve_mixers` first, so reaching this means
         * `attach` was called without reserving room — a programming error.
         */
        throw felspar::stdexcept::logic_error{
                "audio_output::attach would grow the mixer vector past its "
                "reserved capacity while the audio callback may be reading it"};
    }
    m.bind_driver(*drv);
    m.begin();
    mixers.push_back(&m);
    attached.store(mixers.size(), std::memory_order_release);
}


void planet::sdl::audio_output::reconnect(
        std::optional<std::string_view> const device_name) {
    reset();

    static constexpr int iscapture = false;
    char const *chosen_device = nullptr;

    // TODO Search for the wanted audio device in the list and set
    // `chosen_device` to it

    desired.freq = audio::stereo_buffer::samples_per_second;
    desired.format = AUDIO_F32SYS;
    desired.channels = audio::stereo_buffer::channels;
    /**
     * SDL2 requires the requested sample count to be a power of two; the device
     * may return a different value in `spec.samples` after opening, which is
     * what we use to size the mixer blocks below.
     */
    desired.samples = 512;
    desired.callback = audio_callback;
    desired.userdata = this;

    SDL_AudioSpec spec = {};
    device = SDL_OpenAudioDevice(chosen_device, iscapture, &desired, &spec, 0);
    if (device <= 0) {
        throw felspar::stdexcept::runtime_error{"Audio device wouldn't open"};
    }
    /**
     * `SDL_OpenAudioDevice` leaves the device paused; the callback does not
     * fire until `SDL_PauseAudioDevice(device, 0)` below. That window is
     * what lets us safely tear down the previous driver, rebuild it against
     * the negotiated block size, and re-bind every attached mixer (which
     * stops and restarts each producer thread) before the consumer side
     * starts pulling from `next_frame` again.
     */
    drv.emplace(
            static_cast<std::size_t>(spec.samples),
            config.audio_latency_injected_block_count);
    /**
     * Seed the published playback head with the end-time of the first
     * block, so a producer thread that polls the clock before the
     * first callback fires sees the same kind of value it will see
     * between subsequent callbacks.
     */
    drv->playback_head.store(
            audio::sample_clock{
                    static_cast<audio::sample_clock::rep>(spec.samples)},
            std::memory_order_release);
    /**
     * Re-bind every mixer that was attached before this `reconnect`. The
     * device just renegotiated its block size; without this loop those
     * mixers would still hold a `driver const *` to the storage we just
     * re-emplaced and would render slots sized for the previous block.
     * `bind_driver` stops a running producer, resets the ring, and
     * pre-rolls silence; `begin()` restarts the producer. Newly-attached
     * mixers (added via the templated constructor after the first
     * `reconnect`) are handled by `attach` itself.
     */
    {
        std::scoped_lock lock{attach_mtx};
        std::size_t const count = attached.load(std::memory_order_acquire);
        for (std::size_t i{}; i < count; ++i) {
            mixers[i]->bind_driver(*drv);
            mixers[i]->begin();
        }
    }
    SDL_PauseAudioDevice(device, 0);
    std::chrono::milliseconds const buffer_size_ms{
            (spec.samples * 1000) / spec.freq};
    planet::log::info(
            "Requested device", device_name, "opened audio device",
            (chosen_device ? chosen_device : "nullptr"), "- freq:", spec.freq,
            "Hz, samples:", spec.samples, "buffer_size:", buffer_size_ms);
}


namespace {
    /**
     * All of the smoothed callback counters share the same wall-clock
     * half-life so their values stay comparable, e.g. `callback_load` should
     * be close to `callback_duration` times `callback_rate`. The
     * `steady_duration` half-life is counted in readings, so it is converted
     * assuming one reading per default buffer duration.
     */
    auto constexpr c_half_life = 2s;
    std::size_t constexpr c_half_life_callbacks =
            c_half_life / planet::audio::default_buffer_duration;

    planet::telemetry::real_time_rate c_callback_rate{
            "planet_sdl__audio__callback_rate", c_half_life};
    planet::telemetry::steady_duration c_callback_duration{
            "planet_sdl__audio__callback_duration", c_half_life_callbacks};
    planet::telemetry::thread_load c_callback_load{
            "planet_sdl__audio__callback_load", c_half_life};
    planet::telemetry::counter c_clip_count{"planet_sdl__audio__clip_count"};
    planet::telemetry::counter c_underrun_count{
            "planet_sdl__audio__underrun_count"};
}
void planet::sdl::audio_output::audio_callback(
        void *userdata, Uint8 *stream, int len) {
    planet::telemetry::steady_duration::measurement const _{
            c_callback_duration};
    planet::telemetry::thread_load::measurement const _load{c_callback_load};
    c_callback_rate.tick();

    audio_output *const self = reinterpret_cast<audio_output *>(userdata);
    float *const output = reinterpret_cast<float *>(stream);
    std::size_t const wanted =
            len / sizeof(float) / audio::stereo_buffer::channels;

    float const old_master_mul = self->last_master_mul;
    float const target_master_mul = self->config.master_volume.multiplier();
    std::size_t const count = self->attached.load(std::memory_order_acquire);

    /// ### Integrate attached mixers
    /**
     * Each mixer's ring is pre-rolled with silence at construction, so we can
     * consume from `next_frame` unconditionally — there is no startup window to
     * gate against.
     */
    planet::telemetry::counter::value_type clipped{};
    planet::by_index(wanted, [&](std::size_t const sample) {
        std::array<float, audio::stereo_buffer::channels> mix = {};
        for (std::size_t m{}; m < count; ++m) {
            auto const frame = self->mixers[m]->next_frame();
            for (std::size_t ch{}; ch < audio::stereo_buffer::channels; ++ch) {
                mix[ch] += frame[ch];
            }
        }
        float const master_mul = std::lerp(
                old_master_mul, target_master_mul,
                static_cast<float>(sample) / wanted);
        planet::by_index(
                audio::stereo_buffer::channels, [&](std::size_t const channel) {
                    float const value = mix[channel] * master_mul;
                    output[sample * audio::stereo_buffer::channels + channel] =
                            value;
                    if (value > 1.0f or value < -1.0f) { ++clipped; }
                });
    });
    c_clip_count += clipped;
    self->last_master_mul = target_master_mul;

    /// ### Underrun recording
    /// Publish the per-mixer underrun deltas once per callback.
    for (std::size_t m{}; m < count; ++m) {
        auto const total = self->mixers[m]->underrun_count();
        c_underrun_count += total - self->last_underruns[m];
        self->last_underruns[m] = total;
    }

    /// ### Advance the published playback head
    /**
     * The atomic now reflects the end-time of the block the device is
     * about to play *next* — i.e. the deadline by which a mixer producer
     * must have rendered for the upcoming callback.
     */
    auto const advanced =
            self->drv->playback_head.load(std::memory_order_relaxed)
            + audio::sample_clock{
                    static_cast<audio::sample_clock::rep>(wanted)};
    self->drv->playback_head.store(advanced, std::memory_order_release);
}
