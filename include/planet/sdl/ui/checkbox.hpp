#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/checkbox.hpp>


namespace planet::sdl::ui {


    class checkbox final : public planet::ui::checkbox<renderer, texture> {
        using superclass = planet::ui::checkbox<renderer, texture>;

      public:
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

        using constrained_type = superclass::constrained_type;

      private:
        void do_draw(renderer &r) override {
            if (value) {
                on.draw(r);
            } else {
                off.draw(r);
            }
        }
    };


}
