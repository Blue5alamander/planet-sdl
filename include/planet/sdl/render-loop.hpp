#pragma once


#include <planet/sdl/renderer.hpp>


namespace planet::sdl {


    /// Manages the instantiation and destruction of the render loop. Typically
    /// this should be the last member in a structure so it destructed first as
    /// the render loop implementation will typically involve access to a lot of
    /// member data within its structure.
    struct render_loop final {
        template<typename N>
        render_loop(N &o, felspar::coro::stream<renderer::frame> (N::*f)()) {
            current_renderer.post(
                    render_loop::frame_wrapper<N>, this, std::ref(o), f);
        }

        /**
         * When a render function has been connected (see `connect`) the
         * awaitable returned here can be used to wait for the next frame.
         */
        auto next_frame() { return waiting_for_frame.next(); }

      private:
        felspar::coro::eager<> current_renderer;
        felspar::coro::bus<renderer::frame> waiting_for_frame;
        template<typename N>
        static felspar::coro::task<void> frame_wrapper(
                render_loop *r,
                N &o,
                felspar::coro::stream<renderer::frame> (N::*f)()) {
            for (auto frames = (o.*f)(); auto frame = co_await frames.next();) {
                r->waiting_for_frame.push(*frame);
            }
        }
    };


}
