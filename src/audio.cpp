#include <planet/sdl/audio.hpp>

#include <felspar/exceptions.hpp>

#include <mutex>


using namespace std::literals;
using namespace planet::audio::literals;


planet::sdl::audio_output::audio_output(audio::channel &m) : desk{m} {
    int iscapture = {};
    device_name = SDL_GetAudioDeviceName(0, iscapture);

    configuration.freq = audio::stereo_buffer::samples_per_second;
    configuration.format = AUDIO_F32SYS;
    configuration.channels = audio::stereo_buffer::channels;
    configuration.callback = audio_callback;
    configuration.userdata = this;

    SDL_AudioSpec spec = {};
    device = SDL_OpenAudioDevice(
            device_name, iscapture, &configuration, &spec, 0);
    if (device <= 0) {
        throw felspar::stdexcept::runtime_error{"Audio device wouldn't open"};
    } else {
        SDL_PauseAudioDevice(device, 0);
    }
}


planet::sdl::audio_output::~audio_output() {
    if (device > 0) { SDL_CloseAudioDevice(device); }
}


void planet::sdl::audio_output::add_sound_source(audio::stereo_generator sound) {
    std::scoped_lock lock{mtx};
    desk.add_track(std::move(sound));
}


void planet::sdl::audio_output::audio_callback(
        void *userdata, Uint8 *stream, int len) {
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
