#pragma once


#include <planet/audio.hpp>
#include <planet/audio/driver.hpp>
#include <planet/sdl/forward.hpp>
#include <planet/sdl/handle.hpp>

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <optional>
#include <span>

#include <SDL.h>
#undef main


namespace planet::sdl {


    /// ## Audio output
    /**
     * Connect audio sources to this and have them play out through SDL. There
     * should only be a single instance of this type ever active.
     *
     * Each attached `audio::mixer` renders on its own thread into a ring of
     * pre-mixed blocks. The SDL audio callback only sums the ready block from
     * every attached mixer, applies the master gain, and writes the result to
     * the device — it performs no mixing or synthesis itself. After consuming a
     * mixer's block the callback wakes that mixer's producer thread so it stays
     * filled and in sync.
     */
    class audio_output final {
        SDL_AudioDeviceID device = {};
        SDL_AudioSpec desired = {};

        void reset();

        static void audio_callback(void *, Uint8 *, int);


        /// ### Engine configuration
        configuration &config;
        /**
         * The single source for every audio setting: the master gain (read live
         * by the callback), the requested device name, the PulseAudio latency
         * hint, and any audio settings added later. Holding the whole
         * configuration means a new setting needs no constructor or call-site
         * change. Must outlive this `audio_output`.
         */

        /**
         * Attached mixers. Written only by `attach` (under `attach_mtx`) and
         * published via `attached`; read lock-free by the audio callback.
         */
        static constexpr std::size_t max_mixers = 8;
        std::array<audio::mixer *, max_mixers> mixers = {};
        std::atomic<std::size_t> attached = 0;
        std::mutex attach_mtx;


        /// ### Underruns values
        std::array<std::uint64_t, max_mixers> last_underruns = {};
        /**
         * Per-mixer underrun totals as seen at the previous callback;
         * callback-thread-only.
         *
         * An **underrun** happens when the callback asks a mixer for its next
         * block but the mixer's producer thread has not finished rendering one
         * in time: the ring is empty, so silence is emitted in its place and
         * the mixer counts an underrun. It is the symptom of the off-thread
         * render failing to keep pace with the real-time device — typically CPU
         * starvation, a stalled generator, or a `latency` set too low to absorb
         * scheduling jitter. Audibly it is a gap, click, or dropout.
         *
         * Each callback reads every mixer's running total and adds the increase
         * since this snapshot to the `planet_sdl_audio_underrun_count`
         * telemetry counter, then refreshes the snapshot — so the counter
         * reflects underruns across all mixers without double-counting.
         */


        /// ### Gain change monitoring
        float last_master_mul = 1.0f;
        /**
         * Master multiplier at the end of the previous callback;
         * callback-thread-only. Used to ramp the master gain across each buffer
         * so master volume changes don't step (zipper) at buffer boundaries —
         * the per-channel gains are already ramped inside each mixer's
         * `attenuate`.
         */


        /// ### Backend driver
        std::optional<audio::driver> drv;
        /**
         * Backend driver handed to each attached mixer at `attach` time, and
         * re-handed to every already-attached mixer at the end of each
         * `reconnect` (the SDL device is held paused across that window so
         * the consumer side does not race the rebuild).
         *
         * Carries the block size, block count (== mixer ring depth),
         * derived latency, and the audio-clock position the SDL callback
         * will have written up to by the end of the next block it produces.
         *
         * The playback head is seeded in `reconnect` to the negotiated
         * block size so the first reader (before any callback has run) sees
         * the end-time of the first block. The callback advances it after
         * each consume so that, between callbacks, the value is the end of
         * the block the device is about to play next. Read lock-free from
         * any thread; mixers bound via `attach` re-expose it through
         * `mixer::playback_clock()`.
         *
         * Constructed by `reconnect` once `SDL_OpenAudioDevice` has
         * negotiated the actual block size; until then it is empty. Always
         * engaged by the time a constructor or `reconnect` call returns.
         */


      public:
        /// ### Construction/destruction
        explicit audio_output(configuration &);
        /// #### Construct with audio sources
        template<typename... Mixers>
        audio_output(configuration &config, audio::mixer &m, Mixers &...ms)
        : audio_output{config} {
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
        std::atomic<audio::sample_clock> const &playback_clock() const noexcept {
            return drv->playback_head;
        }


      private:
        /// ### Attach a mixer
        void attach(audio::mixer &);
        /**
         * Starts the mixer's producer thread, schedules its stream start, and
         * registers it so the callback sums its output. The mixer must outlive
         * this `audio_output`.
         */
    };


}
