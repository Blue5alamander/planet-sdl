#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/draggable.hpp>


namespace planet::sdl::ui {


    /// ## A draggable UI element
    class draggable final : public planet::ui::draggable<texture> {
        using superclass = planet::ui::draggable<texture>;


      public:
        draggable(renderer &, surface);


      private:
        void do_draw() override;
    };


}
