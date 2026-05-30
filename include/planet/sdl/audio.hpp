#pragma once


#include <planet/audio.hpp>
#include <planet/sdl/handle.hpp>

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>

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
        SDL_AudioSpec configuration = {};

        void reset();

        static void audio_callback(void *, Uint8 *, int);

        /// ### Master gain
        /// The master gain applied over the summed mix in the callback
        audio::channel &master;

        /**
         * Attached mixers. Written only by `attach` (under `attach_mtx`) and
         * published via `attached`; read lock-free by the audio callback.
         */
        static constexpr std::size_t max_mixers = 8;
        std::array<audio::mixer *, max_mixers> mixers = {};
        std::atomic<std::size_t> attached = 0;
        std::mutex attach_mtx;
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
        std::array<std::uint64_t, max_mixers> last_underruns = {};
        /**
         * Master multiplier at the end of the previous callback;
         * callback-thread-only. Used to ramp the master gain across each buffer
         * so master volume changes don't step (zipper) at buffer boundaries —
         * the per-channel gains are already ramped inside each mixer's
         * `attenuate`.
         */
        float last_master_mul = 1.0f;


      public:
        /// ### Construction/destruction
        audio_output(std::optional<std::string_view>, audio::channel &);
        /// #### Construct with audio sources
        template<typename... Mixers>
        audio_output(
                std::optional<std::string_view> const device_name,
                audio::channel &c,
                audio::mixer &m,
                Mixers &...ms)
        : audio_output{device_name, c} {
            attach(m);
            (attach(ms), ...);
        }
        ~audio_output();


        /// ### Switch connection to a different device
        void reconnect(std::optional<std::string_view>);

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
