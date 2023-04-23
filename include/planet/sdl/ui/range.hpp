#pragma once


#include <planet/sdl/ui/draggable.hpp>


namespace planet::sdl::ui {


    class range final : public planet::ui::widget<renderer>, public droppable {
        texture background;
        draggable slider;
        float slider_position = {};

      public:
        range(renderer &, surface bg, surface ctrl);

        using constrained_type = droppable::constrained_type;
        constrained_type reflow(constrained_type const &);

        void
                add_to(planet::ui::baseplate<renderer> &,
                       planet::ui::panel &,
                       float = {}) override;

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
        constrained_type
                drop(constrained_type const &) override;
    };


}
