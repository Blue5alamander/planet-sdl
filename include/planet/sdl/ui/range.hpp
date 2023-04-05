#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/panel.hpp>


namespace planet::sdl::ui {


    class range {
        planet::panel panel;
        texture background, slider;
        bool visible = false;

      public:
        range(renderer &, surface bg, surface ctrl);

        void add_to(planet::panel &);

        affine::extents2d extents(affine::extents2d const &) const;
        void draw_within(renderer &r, affine::rectangle2d);

      private:
        felspar::coro::eager<> response;
        felspar::coro::task<void> drag();
    };


}
