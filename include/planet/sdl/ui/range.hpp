#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/helpers.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    class range final : public planet::ui::widget<renderer> {
        texture background, slider;

      public:
        range(renderer &, surface bg, surface ctrl);

        using constrained_type = planet::ui::constrained2d<float>;
        constrained_type reflow(constrained_type const &);

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
    };

    static_assert(planet::ui::reflowable<range>);


}
