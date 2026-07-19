#pragma once


#include <planet/events/queue.hpp>
#include <planet/sdl/forward.hpp>
#include <planet/ui/baseplate.hpp>

#include <felspar/coro/starter.hpp>


namespace planet::sdl {


    /// ## Event loop
    /**
     * Polls the platform for events, translates each one into the matching
     * `planet::events` type and pushes it onto the relevant queue in `events`.
     * There are two kinds of event and they are consumed in two different ways.
     *
     * The **interactive events** -- `key`, `mouse` and `scroll` -- are the
     * input the UI reacts to. `forward_to_baseplate` forwards them into a
     * `ui::baseplate`'s own queue, so code that handles user input works
     * against the base plate rather than against this event loop.
     *
     * The **window events** -- `focus`, `position` and `resize`, along with
     * `quit` -- describe the window itself rather than anything a widget acts
     * on, so they are deliberately not forwarded. The application consumes them
     * directly from `events`. They are what a program watches to, for example,
     * save and restore the window geometry, or pause rendering while the window
     * is minimised or has lost focus.
     */
    struct event_loop final {
        event_loop(init &s, std::uint32_t const wid) : sdl{s}, window_id{wid} {}
        template<typename W>
        event_loop(init &s, W const &w)
        : event_loop{s, SDL_GetWindowID(w.get())} {}


        /// ### Coroutine tasks to run
        /**
         * These should be run by the application in order for the event loop to
         * be fully and correctly connected to the rest of the application.
         */

        /// #### Run the event loop until the UX is done
        felspar::coro::task<void> run();

        /// #### Forward the interactive events to the base plate running the
        /// widgets
        /**
         * Forwards the `key`, `mouse` and `scroll` queues into the base plate's
         * own `events`. The window events are left for the application to
         * consume directly from `events`.
         */
        felspar::coro::task<void> forward_to_baseplate(ui::baseplate &);


        /// ### Events
        /**
         * The queues fed by `run()`. Interactive events are usually taken via
         * `forward_to_baseplate`; the window events are consumed from here
         * directly.
         */
        events::queue events;


      private:
        planet::sdl::init &sdl;
        std::uint32_t window_id;
    };


}
