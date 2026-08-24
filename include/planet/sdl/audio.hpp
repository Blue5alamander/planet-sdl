#pragma once


#include <planet/audio.hpp>
#include <planet/audio/backend.hpp>
#include <planet/audio/output.hpp>
#include <planet/sdl/forward.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/sdl/sdl.hpp>

#include <atomic>
#include <optional>
#include <span>


namespace planet::sdl {


    /// ## Audio output
    /**
     * Connect audio sources to this and have them play out through SDL. There
     * should only be a single instance of this type ever active.
     *
     * This is the SDL implementation of `planet::audio::backend`. It owns the
     * device stream and delegates all mixing to a `planet::audio::output` core:
     * each attached `audio::mixer` renders on its own thread into a ring of
     * pre-mixed blocks, and the SDL audio callback only asks the core to render
     * one block and queues the result on the stream — it performs no mixing or
     * synthesis itself.
     */
    class audio_output final : public audio::backend {
        handle<SDL_AudioStream, SDL_DestroyAudioStream> stream;
        /**
         * Stream bound to the playback device opened by
         * `SDL_OpenAudioDeviceStream`. Destroying the stream also closes that
         * device, quiescing the callback.
         */

        static void audio_callback(void *, SDL_AudioStream *, int, int);

        void reset();


        /// ### Engine configuration
        configuration &config;
        /**
         * The single source for every audio setting: the master gain (read live
         * by the core), the requested device name, the PulseAudio latency hint,
         * and any audio settings added later. Must outlive this `audio_output`.
         */


        /// ### Shared mixing core
        audio::output core;
        /**
         * Owns the driver, the attached mixers, and the render body shared by
         * every backend. The SDL callback drives it through `output::render`,
         * and `reconnect` rebuilds its driver for the negotiated block size.
         * The master-gain accessor it renders through reads the configuration
         * live, so a volume change is picked up on the next callback.
         */


        /// ### Samples per rendered block
        std::size_t block_size = 0;
        /**
         * The block size the callback renders and queues in, set by `reconnect`
         * from `config.default_buffer_duration` while the stream is held
         * paused. The callback only starts after `reconnect` resumes the
         * stream, so it always reads a settled value.
         */


      public:
        /// ### Construction/destruction
        explicit audio_output(configuration &);
        /// #### Construct with audio sources
        template<typename... Mixers>
        audio_output(configuration &config, audio::mixer &m, Mixers &...ms)
        : audio_output{config} {
            reserve_mixers(1 + sizeof...(ms));
            attach(m);
            (attach(ms), ...);
        }
        /// #### Construct from a span of audio sources
        /**
         * Attaches every mixer in `ms`, in order. Equivalent to the variadic
         * constructor but for a run-time-sized collection, so a caller holding
         * a `std::span` of mixers need not expand it into an argument pack.
         * Each mixer must outlive this `audio_output`.
         */
        audio_output(configuration &config, std::span<audio::mixer> const ms)
        : audio_output{config} {
            reserve_mixers(ms.size());
            for (auto &m : ms) { attach(m); }
        }
        ~audio_output();


        /// ### Switch connection to a different device
        void reconnect(std::optional<std::string_view>);


        /// ### Playback head
        /**
         * The SDL audio callback advertises the audio-clock position it
         * will have written through by the end of the next block it
         * produces. Use this to schedule new audio events relative to the
         * playback head with no `steady_clock`→`sample_clock` rounding.
         */
        std::atomic<audio::sample_clock> const &
                playback_clock() const noexcept override {
            return core.playback_clock();
        }


      private:
        /// ### Reserve storage for the mixers attached at construction
        void reserve_mixers(std::size_t);
        /**
         * Called once from each mixer-taking constructor before the `attach`
         * loop, with the exact number of mixers about to be attached, so the
         * core can fix its capacity before the live callback can read it.
         */

        /// ### Attach a mixer
        void attach(audio::mixer &);
        /**
         * Registers the mixer with the core so the callback sums its output.
         * The mixer must outlive this `audio_output`.
         */
    };


}
