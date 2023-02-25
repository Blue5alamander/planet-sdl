#pragma once


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
        event_loop(planet::sdl::init &s, sdl::window &w) : sdl{s}, window{w} {}

        felspar::coro::task<void> run();

      private:
        planet::sdl::init &sdl;
        sdl::window &window;
    };


}
