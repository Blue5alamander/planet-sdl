#pragma once


#include <planet/sdl/ui/draggable.hpp>
#include <planet/ui/range.hpp>


namespace planet::sdl::ui {


    /// ## Range/slide control
    class range final : public planet::ui::range<renderer, texture, draggable> {
        using superclass = planet::ui::range<renderer, texture, draggable>;

        void do_draw(renderer &) override;


      public:
        range(renderer &,
              surface bg,
              surface ctrl,
              planet::ui::constrained1d<float> const &position);
        range(std::string_view,
              renderer &,
              surface bg,
              surface ctrl,
              planet::ui::constrained1d<float> const &position);
    };


}
