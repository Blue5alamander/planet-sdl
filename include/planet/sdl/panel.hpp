#pragma once


#include <planet/affine2d.hpp>

#include <felspar/coro/bus.hpp>
#include <felspar/coro/eager.hpp>


namespace planet::sdl {


    class renderer;
    class texture;


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
        panel &rotate(float const t) {
            viewport.rotate(t);
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
        /// Scale the texture, drawing it to fill the rectangle
        void copy(texture const &, affine::rectangle) const;
    };


}
