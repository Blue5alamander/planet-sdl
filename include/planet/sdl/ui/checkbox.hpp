#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/panel.hpp>


namespace planet::sdl::ui {


    class checkbox {
        planet::panel panel;
        texture on, off;
        bool &value;
        bool visible = false;

      public:
        checkbox(renderer &r, surface on, surface off, bool &v)
        : on{r, std::move(on)}, off{r, std::move(off)}, value{v} {}

        void add_to(planet::panel &parent) {
            parent.add_child(panel);
            response.post(*this, &checkbox::button_response);
            visible = true;
        }

        affine::extents2d extents(affine::extents2d const &ex) const {
            auto const on_size = on.extents(ex);
            auto const off_size = off.extents(ex);
            return {std::max(on_size.width, off_size.width),
                    std::max(on_size.height, off_size.height)};
        }
        void draw_within(renderer &r, affine::rectangle2d const outer) {
            if (visible) {
                if (value) {
                    on.draw_within(r, outer);
                    panel.move_to({outer.top_left, on.extents()});
                } else {
                    off.draw_within(r, outer);
                    panel.move_to({outer.top_left, off.extents()});
                }
            }
        }

      private:
        felspar::coro::eager<> response;
        felspar::coro::task<void> button_response() {
            while (true) {
                co_await panel.clicks.next();
                value = not value;
            }
        }
    };


}
