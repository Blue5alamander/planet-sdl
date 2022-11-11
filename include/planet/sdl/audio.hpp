#pragma once


#include <planet/audio.hpp>
#include <planet/sdl/handle.hpp>

#include <thread>

#include <SDL.h>


namespace planet::sdl {


    /// Stereo output buffer type
    using stereo = audio::buffer_storage<audio::sample_clock, 2>;
    using stereo_generator = felspar::coro::generator<stereo>;


    /// Connect an audio source to this and have it play out through SDL
    class audio_output final {
        SDL_AudioDeviceID device = {};
        SDL_AudioSpec configuration = {};

        /// All of these items are accessed from the SDL audio thread
        std::mutex mtx;
        static void audio_callback(void *, Uint8 *, int);
        audio::mixer<stereo> desk;
        stereo_generator desk_output = desk.output();
        felspar::memory::holding_pen<stereo> playing;
        std::size_t playing_marker = {};

      public:
        audio_output();
        ~audio_output();

        char const *device_name = nullptr;
        audio::linear_gain master;

        /// Play this audio starting as soon as possible
        void trigger(stereo_generator);
    };


}
