#pragma once


#include <planet/events.hpp>

#include <felspar/coro/bus.hpp>
#include <felspar/coro/eager.hpp>
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

        felspar::coro::task<void> run();

        /// ## Event busses
        felspar::coro::bus<events::key> key;
        felspar::coro::bus<events::mouse> mouse;
        felspar::coro::bus<events::quit> quit;
        felspar::coro::bus<events::scroll> scroll;

      private:
        planet::sdl::init &sdl;
        std::uint32_t window_id;
    };


}
