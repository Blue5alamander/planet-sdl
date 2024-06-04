#pragma once


#include <planet/sdl/renderer.hpp>

#include <iostream>


namespace planet::sdl {


    /// ## Render loop
    /**
     * Manages the instantiation and destruction of the render loop. It should
     * be passed an object and member that implements the rendering loop stream
     * and this will ensure that the render loop is properly stopped and
     * destructed when the owning object of the `render_loop` instance is
     * destructed.
     *
     * Typically this should be the last member in a structure so it destructed
     * first as the render loop implementation will typically involve access to
     * a lot of member data within its structure.
     *
     * **NB** This is used only for the SDL renderer. If you're using the
     * Planet's Vulkan engine then you will want to use it's [`XXX`](XXX) type.
     */
    struct render_loop final {
        template<typename N>
        render_loop(N &o, felspar::coro::stream<renderer::frame> (N::*f)()) {
            current_renderer.post(
                    render_loop::frame_wrapper<N>, this, std::ref(o), f);
        }

        auto next_frame() { return waiting_for_frame.next(); }

      private:
        felspar::coro::eager<> current_renderer;
        felspar::coro::bus<renderer::frame> waiting_for_frame;
        template<typename N>
        static felspar::coro::task<void> frame_wrapper(
                render_loop *r,
                N &o,
                felspar::coro::stream<renderer::frame> (N::*f)()) {
            try {
                for (auto frames = (o.*f)();
                     auto frame = co_await frames.next();) {
                    r->waiting_for_frame.push(*frame);
                }
            } catch (std::exception const &e) {
                std::cerr << "Render loop exception caught: " << e.what()
                          << std::endl;
                std::terminate();
            }
        }
    };


}
