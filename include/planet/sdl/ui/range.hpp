#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    class range final : public planet::ui::widget<renderer> {
        texture background, slider;

      public:
        range(renderer &, surface bg, surface ctrl);

        affine::extents2d extents(affine::extents2d const &) const;

      private:
        void do_draw_within(renderer &r, affine::rectangle2d) override;
        felspar::coro::task<void> behaviour() override;
    };


}
