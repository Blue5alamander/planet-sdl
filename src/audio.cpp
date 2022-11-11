#include <planet/sdl/audio.hpp>

#include <felspar/exceptions.hpp>


using namespace std::literals;
using namespace planet::audio::literals;


planet::sdl::audio_output::audio_output(stereo_generator background)
: master{-6_dB} {
    desk.add_track(std::move(background));

    int iscapture = {};
    device_name = SDL_GetAudioDeviceName(0, iscapture);

    configuration.freq = stereo::samples_per_second;
    configuration.format = AUDIO_F32SYS;
    configuration.channels = stereo::channels;
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


void planet::sdl::audio_output::audio_callback(
        void *userdata, Uint8 *stream, int len) {
    audio_output *const self = reinterpret_cast<audio_output *>(userdata);
    float *const output = reinterpret_cast<float *>(stream);
    std::size_t const wanted = len / sizeof(float) / stereo::channels;

    for (std::size_t sample{}; sample < wanted; ++sample) {
        if (not self->playing
            or self->playing_marker >= self->playing->samples()) {
            self->playing = self->desk_output.next();
            self->playing_marker = {};
        }
        for (std::size_t channel{}; channel < stereo::channels; ++channel) {
            output[sample * stereo::channels + channel] =
                    (*self->playing)[self->playing_marker][channel];
        }
        ++self->playing_marker;
    }
}
