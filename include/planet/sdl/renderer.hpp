#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>

#include <felspar/coro/bus.hpp>
#include <felspar/coro/eager.hpp>
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

        /// Transformation into and out of the coordinate space
        affine::transform viewport = {};

        struct child final {
            affine::point2d top_left, bottom_right;
            panel *sub;

            child(panel *, affine::point2d, affine::point2d);
        };
        std::vector<child> children;

        // Swap out the current parent with a new one
        void reparent_children(panel *);
        // Feed children events
        felspar::coro::eager<> feeder;
        felspar::coro::task<void> feed_children();

      public:
        panel();
        panel(renderer &);
        ~panel();

        /// Given the way we want to be able to use panels, we have to be able
        /// to ensure stable addresses, so: not movable and not copyable
        panel(panel const &) = delete;
        panel(panel &&) = delete;
        panel &operator=(panel const &) = delete;
        panel &operator=(panel &&) = delete;


        /// ## Co-ordinate space for this panel

        /// Remove all transformations into and out of the coordinate space
        void reset_coordinate_space() { viewport = {}; }

        /// Forward to the viewport taking the hierarchy into account. When
        /// going into the screen co-ordinate space we must apply our local
        /// transformations before we apply the parent ones. When coming out of
        /// the screen coordinates we have to undo the parent transformation
        /// first
        affine::point2d into(affine::point2d const p) {
            if (parent) {
                return parent->into(viewport.into(p));
            } else {
                return viewport.into(p);
            }
        }
        affine::point2d outof(affine::point2d const p) {
            if (parent) {
                return viewport.outof(parent->outof(p));
            } else {
                return viewport.outof(p);
            }
        }

        panel &reflect_y() {
            viewport.reflect_y();
            return *this;
        }
        panel &translate(affine::point2d const p) {
            viewport.translate(p);
            return *this;
        }
        panel &scale(float const s) {
            viewport.scale(s);
            return *this;
        }


        /// ## Panel hierarchy management

        /// Add a child covering the requested portion of the parent in the
        /// panel's coordinate space
        void add_child(
                panel &, affine::point2d top_left, affine::point2d bottom_right);
        /// Remove the passed child from the panel
        void remove_child(panel &);


        /// ## Message delivery and focus

        /// Left button release position
        felspar::coro::bus<affine::point2d> mouse_click;


        /// ## Drawing in the panel coordinate space

        /// Draw a line between two points in world co-ordinate space
        void line(affine::point2d, affine::point2d) const;
        /// Draw the texture at native size with its top left corder at the
        /// location specified
        void copy(texture const &, affine::point2d) const;
    };


    class renderer final {
        window &win;
        handle<SDL_Renderer, SDL_DestroyRenderer> pr;
        /// Handle the optional render coroutine support
        felspar::coro::eager<> current_renderer;

      public:
        renderer(window &);

        SDL_Renderer *get() const noexcept { return pr.get(); }


        /// ## Render function

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
        /// When a render function has been connected (see `connect`) the
        /// awaitable returned here can be used to wait for the next frame
        auto next_frame() { return waiting_for_frame.next(); }


        /// ## Frame presentation

        /// Clear draw commands ready for next frame
        void clear() const;
        /// Send the current draw commands to the screen
        void present() const;


        /// ## Graphics APIs
        /**
         * All of these APIs use the SDL coordinate space, with no adjustment
         * form `screen` member coordinate space.
         */

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
