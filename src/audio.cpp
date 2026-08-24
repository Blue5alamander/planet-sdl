#include <planet/log.hpp>
#include <planet/sdl/audio.hpp>
#include <planet/sdl/init.hpp>
#include <planet/telemetry/duration.hpp>
#include <planet/telemetry/load.hpp>
#include <planet/telemetry/rate.hpp>

#include <felspar/exceptions/runtime_error.hpp>


using namespace std::literals;
using namespace planet::audio::literals;


static_assert(
        std::atomic<planet::audio::sample_clock>::is_always_lock_free,
        "Playback-head atomic must be lock-free on the real-time audio thread");


/// ## `planet::sdl::audio_output`


namespace {
    /// ### Deleter for the SDL3 audio playback device list, as in `init`
    void free_audio_device_list(SDL_AudioDeviceID *const devices) noexcept
    /**
     * `SDL_GetAudioPlaybackDevices` hands back an `SDL_free`-owned array. That
     * takes `void *` while `planet::sdl::handle` wants `void(T *)`, so this
     * adapts the two and lets the list be owned by a `handle`.
     */
    {
        SDL_free(devices);
    }
}


planet::sdl::audio_output::audio_output(configuration &config)
: config{config},
  core{[&config] { return config.master_volume.multiplier(); }} {
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
    /**
     * The stream was created by `SDL_OpenAudioDeviceStream`, so destroying it
     * also closes the device that was opened alongside it. SDL takes the
     * stream's lock during destruction, so a callback in flight finishes
     * before the stream goes away.
     */
    stream.reset();
}


void planet::sdl::audio_output::reserve_mixers(std::size_t const count) {
    core.reserve_mixers(count);
}


void planet::sdl::audio_output::attach(audio::mixer &m) { core.attach(m); }


void planet::sdl::audio_output::reconnect(
        std::optional<std::string_view> const device_name) {
    reset();

    /**
     * Resolve the requested device name to a playback device ID. The list is
     * enumerated fresh here rather than cached from start up because the set of
     * devices changes over the life of the program (headphones plugged in, a
     * monitor woken up). An empty name — or a name that no longer matches any
     * present device — falls back to the system default, so a device that has
     * since gone away still leaves the game with sound.
     */
    SDL_AudioDeviceID device_id = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
    if (device_name) {
        int device_count = 0;
        handle<SDL_AudioDeviceID, free_audio_device_list> const devices{
                SDL_GetAudioPlaybackDevices(&device_count)};
        for (int device = 0; device < device_count; ++device) {
            char const *const dn =
                    SDL_GetAudioDeviceName(devices.get()[device]);
            if (dn and *device_name == dn) {
                device_id = devices.get()[device];
                break;
            }
        }
        if (device_id == SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK) {
            planet::log::warning(
                    "Requested audio device not found, using the default "
                    "instead",
                    device_name);
        }
    }

    SDL_AudioSpec spec = {};
    spec.format = SDL_AUDIO_F32;
    spec.channels = static_cast<int>(audio::stereo_buffer::channels);
    spec.freq = static_cast<int>(audio::stereo_buffer::samples_per_second);
    stream = SDL_OpenAudioDeviceStream(device_id, &spec, audio_callback, this);
    if (not stream.get()) {
        throw felspar::stdexcept::runtime_error{"Audio device wouldn't open"};
    }
    /**
     * `SDL_OpenAudioDeviceStream` opens the device paused; the stream's get
     * callback does not fire until `SDL_ResumeAudioStreamDevice` below. That
     * window is what lets us safely tear down the previous driver, rebuild it
     * against the chosen block size, and re-bind every attached mixer (which
     * `output::rebuild_driver` does) before the consumer side starts pulling
     * from `next_frame` again.
     */
    /**
     * The block size the mixer rings and the playback head advance in, read
     * from the configuration and clamped at load to lie in `(0,
     * audio::max_buffer_duration]`. The stream callback renders whole blocks of
     * this size and queues them on the stream with `SDL_PutAudioStreamData`.
     */
    block_size =
            static_cast<std::size_t>(config.default_buffer_duration.count());
    core.rebuild_driver(block_size, config.audio_latency_injected_block_count);

    if (not SDL_ResumeAudioStreamDevice(stream.get())) {
        throw felspar::stdexcept::runtime_error{
                "Audio device stream wouldn't resume"};
    }
    std::chrono::milliseconds const buffer_size_ms{
            (block_size * 1000) / static_cast<std::size_t>(spec.freq)};
    planet::log::info(
            "Connected audio output. Requested device", device_name,
            "- freq:", spec.freq, "Hz, samples:", block_size,
            "buffer_size:", buffer_size_ms);
}


namespace {
    /**
     * All of the smoothed callback counters share the same wall-clock half-life
     * so their values stay comparable, e.g. `callback_load` should be close to
     * `callback_duration` times `callback_rate`. The `steady_duration`
     * half-life is counted in readings, so it is converted from wall-clock time
     * to a reading count assuming one reading per buffer. That counter is a
     * namespace-scope static constructed before any configuration loads, so it
     * cannot read the runtime block size; the reading count therefore assumes
     * the initial working block, i.e. one callback every
     * `initial_buffer_duration` samples.
     *
     * These measure the SDL callback itself, so they stay named for this
     * backend; the render-body counters (clip, underrun) live in
     * `planet::audio::output` under backend-neutral names.
     */
    auto constexpr c_half_life = 2s;
    std::size_t constexpr c_half_life_callbacks =
            c_half_life / planet::audio::initial_buffer_duration;

    planet::telemetry::real_time_rate c_callback_rate{
            "planet_sdl__audio__callback_rate", c_half_life};
    planet::telemetry::steady_duration c_callback_duration{
            "planet_sdl__audio__callback_duration", c_half_life_callbacks};
    planet::telemetry::thread_load c_callback_load{
            "planet_sdl__audio__callback_load", c_half_life};
}


void planet::sdl::audio_output::audio_callback(
        void *const userdata,
        SDL_AudioStream *const stream,
        int const additional_amount,
        int const) {
    planet::telemetry::steady_duration::measurement const _{
            c_callback_duration};
    planet::telemetry::thread_load::measurement const _load{c_callback_load};
    c_callback_rate.tick();

    audio_output *const self = reinterpret_cast<audio_output *>(userdata);
    /**
     * The stream asks for however many bytes it needs to satisfy the device;
     * render whole app-side blocks (rounding the request up) so the mixer rings
     * and the playback head always advance in the driver's block size. Any
     * surplus stays queued in the stream and is deducted from the next request.
     * `output::render` flushes denormals for this real-time thread, so the
     * callback does not repeat that itself.
     */
    std::size_t const block_size = self->block_size;
    std::array<float, audio::max_buffer_samples * audio::stereo_buffer::channels>
            buffer;
    std::size_t const buffer_bytes =
            block_size * audio::stereo_buffer::channels * sizeof(float);
    for (int remaining = additional_amount; remaining > 0;
         remaining -= static_cast<int>(buffer_bytes)) {
        self->core.render(buffer.data(), block_size);
        SDL_PutAudioStreamData(
                stream, buffer.data(), static_cast<int>(buffer_bytes));
    }
}
