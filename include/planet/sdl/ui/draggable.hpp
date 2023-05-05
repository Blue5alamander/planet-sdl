#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    /// ## Droppable target
    /**
     * An abstract super class for types that will respond to the "drop" part of
     * the draggable sequence. This happens when the mouse button is released
     * after dragging to a new position.
     */
    struct droppable {
        using constrained_type = planet::ui::constrained2d<float>;

        /// Respond to the end of a drag
        /**
         * The returned constraint becomes the new offset in the draggable
         * widget. Nearly always this should be zero so that new drags start at
         * the current position rather than random one.
         */
        virtual constrained_type drop(constrained_type const &) = 0;
    };


    /// ## A draggable UI element
    class draggable final : public planet::ui::widget<renderer> {
        texture hotspot;

      public:
        draggable(renderer &, surface);

        using constrained_type = droppable::constrained_type;

        droppable *target = nullptr;
        constrained_type offset;

        void
                draw(renderer &r,
                     felspar::source_location const &loc =
                             felspar::source_location::current()) {
            hotspot.draw(r, loc);
        }

      private:
        constrained_type do_reflow(constrained_type const &) override;
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
    };


}
