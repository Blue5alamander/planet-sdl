#pragma once


#include <planet/events/bus.hpp>

#include <felspar/coro/task.hpp>


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

        /// Run the event loop until the UX is done
        felspar::coro::task<void> run();

        /// ## Event busses
        events::bus bus;

      private:
        planet::sdl::init &sdl;
        std::uint32_t window_id;
    };


}
