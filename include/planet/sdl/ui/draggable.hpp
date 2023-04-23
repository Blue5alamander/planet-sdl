#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    /// ## A draggable UI element
    class draggable final : public planet::ui::widget<renderer> {
        texture hotspot;

      public:
        draggable(renderer &, surface, affine::point2d const &);

        affine::point2d offset;

        using constrained_type = planet::ui::constrained2d<float>;
        constrained_type reflow(constrained_type const &);

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
    };


}
