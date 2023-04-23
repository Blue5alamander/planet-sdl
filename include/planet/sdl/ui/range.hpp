#pragma once


#include <planet/sdl/ui/draggable.hpp>


namespace planet::sdl::ui {


    class range final : public planet::ui::widget<renderer> {
        texture background;
        draggable slider;

      public:
        range(renderer &, surface bg, surface ctrl);

        using constrained_type = planet::ui::constrained2d<float>;
        constrained_type reflow(constrained_type const &);

        void
                add_to(planet::ui::baseplate<renderer> &,
                       planet::ui::panel &,
                       float = {}) override;

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
    };


}
