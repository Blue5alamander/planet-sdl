#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>

#include <felspar/coro/bus.hpp>
#include <felspar/coro/start.hpp>
#include <felspar/coro/stream.hpp>

#include <SDL.h>

#include <cstdint>
#include <span>


namespace planet::sdl {


    class renderer;
    class texture;
    class window;


    /// Co-ordinate transform hierarchy for world etc. co-ordinate systems
    class panel final {
        renderer *rend = nullptr;
        panel *parent = nullptr;

        struct child final {
            affine::point2d top_left, bottom_right;
            panel *sub;

            child(panel *, affine::point2d, affine::point2d);
        };
        std::vector<child> children;

        // Swap out the current parent with a new one
        void reparent_children(panel *);

      public:
        panel() {}
        panel(renderer &);
        ~panel();

        /// Transformation into and out of the coordinate space
        affine::transform viewport = {};

        /// ## Panel hierarchy management

        /// Add a child covering the requested portion of the parent in the
        /// panel's coordinate space
        void add_child(
                panel &, affine::point2d top_left, affine::point2d bottom_right);
        /// Remove the passed child from the panel
        void remove_child(panel &);

        /// ## Drawing in the panel coordinate space

        /// Draw a line between two points in world co-ordinate space
        void line(affine::point2d, affine::point2d) const;
    };


    class renderer final {
        window &win;
        handle<SDL_Renderer, SDL_DestroyRenderer> pr;
        /// Handle the optional render coroutine support
        felspar::coro::starter<> current_renderer;

      public:
        renderer(window &);

        SDL_Renderer *get() const noexcept { return pr.get(); }

        /// Describe the last frame that has been rendered
        struct frame {
            std::size_t number;
        };

        /// Set a function that controls rendering and yields a frame structure
        /// after drawing the frame
        template<typename N>
        void connect(N &o, felspar::coro::stream<frame> (N::*f)()) {
            current_renderer.post(
                    renderer::frame_wrapper<N>, this, std::ref(o), f);
        }
        /// When using render function is connected frames can be awaited by
        /// calling `next_frame`
        auto next_frame() { return waiting_for_frame.next(); }

        /// Clear draw commands ready for next frame
        void clear() const;
        /// Send the current draw commands to the screen
        void present() const;

        /// Graphics APIs in pixel coordinate space
        void colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) const;
        void
                line(std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2) const;
        void lines(std::span<SDL_Point>) const;
        void copy(texture const &, std::size_t x, std::size_t y);

      private:
        felspar::coro::bus<frame> waiting_for_frame;
        template<typename N>
        static felspar::coro::task<void> frame_wrapper(
                renderer *r, N &o, felspar::coro::stream<frame> (N::*f)()) {
            for (auto frames = (o.*f)(); auto frame = co_await frames.next();) {
                r->waiting_for_frame.push(*frame);
            }
        }
    };


}
