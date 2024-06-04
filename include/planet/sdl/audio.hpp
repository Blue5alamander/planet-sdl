#pragma once


#include <planet/audio.hpp>
#include <planet/sdl/handle.hpp>

#include <mutex>
#include <thread>

#include <SDL.h>
#undef main


namespace planet::sdl {


    /// ## Audio output
    /// Connect an audio source to this and have it play out through SDL
    class audio_output final {
        SDL_AudioDeviceID device = {};
        SDL_AudioSpec configuration = {};

        /// All of these items are accessed from the SDL audio thread
        std::mutex mtx;
        static void audio_callback(void *, Uint8 *, int);
        audio::mixer desk;
        audio::stereo_generator desk_output = desk.output();
        felspar::memory::holding_pen<audio::stereo_buffer> playing;
        std::size_t playing_marker = {};

      public:
        audio_output();
        ~audio_output();

        char const *device_name = nullptr;
        audio::atomic_linear_gain master;

        /// Play this audio starting as soon as possible
        void trigger(audio::stereo_generator);
    };


}
