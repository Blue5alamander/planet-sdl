#pragma once


#include <planet/events/queue.hpp>
#include <planet/ui/baseplate.hpp>

#include <felspar/coro/start.hpp>


namespace planet::sdl {


    class init;
    class window;


    /// ## Event loop
    /**
     * This will handle and event loop sending all of the mouse events to the
     * window's panel from which UI elements which need them can get them.
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

        /// #### Forward all events to the base plate running the widgets
        felspar::coro::task<void> forward_to_baseplate(ui::baseplate &);


        /// ### Events
        events::queue events;


      private:
        planet::sdl::init &sdl;
        std::uint32_t window_id;
    };


}
