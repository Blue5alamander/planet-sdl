#pragma once


#include <planet/sdl/ui/draggable.hpp>


namespace planet::sdl::ui {


    /// ## Range/slide control
    class range final : public planet::ui::widget<renderer>, public droppable {
        texture background;
        draggable slider;
        float px_offset = {};

      public:
        range(renderer &,
              surface bg,
              surface ctrl,
              planet::ui::constrained1d<float> const &position);

        planet::ui::constrained1d<float> slider_position = {};

        using constrained_type = droppable::constrained_type;
        constrained_type reflow(constrained_type const &) override;

        void
                add_to(planet::ui::baseplate<renderer> &,
                       planet::ui::panel &,
                       float = {}) override;

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
        constrained_type drop(constrained_type const &) override;
    };


}
