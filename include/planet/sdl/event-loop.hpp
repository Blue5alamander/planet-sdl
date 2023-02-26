#pragma once


#include <planet/events/mouse.hpp>
#include <planet/events/quit.hpp>

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
        event_loop(planet::sdl::init &s, std::uint32_t const wid)
        : sdl{s}, window_id{wid} {}

        felspar::coro::task<void> run();

        /// ## Event busses
        felspar::coro::bus<planet::events::quit> quit;
        felspar::coro::bus<events::mouse> raw_mouse;

      private:
        planet::sdl::init &sdl;
        std::uint32_t window_id;
    };


}
