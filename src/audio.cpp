#include <planet/log.hpp>
#include <planet/sdl/audio.hpp>
#include <planet/telemetry/rate.hpp>

#include <felspar/exceptions.hpp>

#include <mutex>


using namespace std::literals;
using namespace planet::audio::literals;


planet::sdl::audio_output::audio_output(
        std::optional<std::string_view> const device_name, audio::channel &m)
: desk{m} {
    reconnect(device_name);
}


planet::sdl::audio_output::~audio_output() { reset(); }


void planet::sdl::audio_output::reset() {
    if (device > 0) { SDL_CloseAudioDevice(std::exchange(device, 0)); }
}


void planet::sdl::audio_output::add_sound_source(audio::stereo_generator sound) {
    std::scoped_lock lock{mtx};
    desk.add_track(std::move(sound));
}


void planet::sdl::audio_output::reconnect(
        std::optional<std::string_view> const device_name) {
    reset();

    static constexpr int iscapture = false;
    char const *chosen_device = nullptr;

    // TODO Search for the wanted audio device in the list and set
    // `chosen_device` to it

    configuration.freq = audio::stereo_buffer::samples_per_second;
    configuration.format = AUDIO_F32SYS;
    configuration.channels = audio::stereo_buffer::channels;
    configuration.callback = audio_callback;
    configuration.userdata = this;

    SDL_AudioSpec spec = {};
    device = SDL_OpenAudioDevice(
            chosen_device, iscapture, &configuration, &spec, 0);
    if (device <= 0) {
        throw felspar::stdexcept::runtime_error{"Audio device wouldn't open"};
    } else {
        SDL_PauseAudioDevice(device, 0);
    }
    planet::log::info(
            "Requested device", device_name, "opened audio device",
            (chosen_device ? chosen_device : "nullptr"));
}


namespace {
    planet::telemetry::real_time_rate c_callback_rate{
            "planet_sdl_audio_callback_rate", 1s};
}
void planet::sdl::audio_output::audio_callback(
        void *userdata, Uint8 *stream, int len) {
    c_callback_rate.tick();
    audio_output *const self = reinterpret_cast<audio_output *>(userdata);
    float *const output = reinterpret_cast<float *>(stream);
    std::size_t const wanted =
            len / sizeof(float) / audio::stereo_buffer::channels;

    std::scoped_lock lock{self->mtx};
    for (std::size_t sample{}; sample < wanted; ++sample) {
        if (not self->playing
            or self->playing_marker >= self->playing->samples()) {
            self->playing = self->desk_output.next();
            self->playing_marker = {};
        }
        for (std::size_t channel{}; channel < audio::stereo_buffer::channels;
             ++channel) {
            output[sample * audio::stereo_buffer::channels + channel] =
                    (*self->playing)[self->playing_marker][channel];
        }
        ++self->playing_marker;
    }
}
