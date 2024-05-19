#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/checkbox.hpp>


namespace planet::sdl::ui {


    class checkbox final : public planet::ui::checkbox<texture> {
        using superclass = planet::ui::checkbox<texture>;


      public:
        using constrained_type = superclass::constrained_type;


        explicit checkbox(renderer &r, surface on, surface off, bool &v)
        : superclass{
                  "planet::sdl::ui::checkbox",
                  {r, std::move(on)},
                  {r, std::move(off)},
                  {v}} {}
        explicit checkbox(
                std::string_view const n,
                renderer &r,
                surface on,
                surface off,
                bool &v)
        : superclass{n, {r, std::move(on)}, {r, std::move(off)}, {v}} {}


      private:
        void do_draw() override {
            if (value) {
                on.draw();
            } else {
                off.draw();
            }
        }
    };


}
