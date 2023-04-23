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
         * widget.
         */
        virtual constrained_type drop(constrained_type const &) = 0;
    };


    /// ## A draggable UI element
    class draggable final : public planet::ui::widget<renderer> {
        texture hotspot;

      public:
        draggable(renderer &, surface);

        using constrained_type = droppable::constrained_type;
        constrained_type reflow(constrained_type const &);

        droppable *target = nullptr;
        constrained_type offset;

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
    };


}
