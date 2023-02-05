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
     *
     * Pass it the coroutine that implements the user interface for the
     * application. The `event_loop` coroutine `run` can be awaited in the
     * `sdl::co_main` coroutine. It will exit either when the event loop sees an
     * application quit message, or when the user interface coroutine is done.
     */
    struct event_loop final {
        template<typename N>
        event_loop(
                planet::sdl::init &s,
                sdl::window &w,
                N &o,
                felspar::coro::task<void> (N::*f)())
        : sdl{s}, window{w} {
            ui.post(o, f);
        }

        felspar::coro::task<int> run();

      private:
        planet::sdl::init &sdl;
        sdl::window &window;
        felspar::coro::eager<> ui;
    };


}
