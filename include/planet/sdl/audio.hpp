#pragma once


#include <planet/audio.hpp>
#include <planet/sdl/handle.hpp>

#include <mutex>
#include <thread>

#include <SDL.h>
#undef main


namespace planet::sdl {


    /// ## Audio output
    /**
     * Connect an audio source to this and have it play out through SDL. There
     * should only be a single instance of this type ever active with other
     * audio channels used for different types of audio. These should have their
     * own gain controls, with the master gain control being used here on the
     * final output.
     */
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
        /// ### Construction
        audio_output(audio::channel &);
        /// #### Construct with audio sources
        template<typename Source, typename... Sources>
        audio_output(audio::channel &c, Source &s, Sources &...ss)
        : audio_output{c} {
            desk.add_track(s.output());
            (desk.add_track(ss.output()), ...);
        }
        ~audio_output();


        char const *device_name = nullptr;


        /// ## Adds a new sound source
        /**
         * Normally prefer to use the constructor that takes the sound sources.
         * A game will generally know exactly which main audio outputs are to be
         * mixed together.
         */
        void add_sound_source(audio::stereo_generator);
    };


}
