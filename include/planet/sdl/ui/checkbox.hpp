#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    class checkbox final : public planet::ui::widget<renderer> {
        texture on, off;
        bool &value;

      public:
        checkbox(renderer &r, surface on, surface off, bool &v)
        : on{r, std::move(on)}, off{r, std::move(off)}, value{v} {}

        affine::extents2d extents(affine::extents2d const &ex) const {
            auto const on_size = on.extents(ex);
            auto const off_size = off.extents(ex);
            return {std::max(on_size.width, off_size.width),
                    std::max(on_size.height, off_size.height)};
        }

      private:
        void do_draw_within(
                renderer &r, affine::rectangle2d const outer) override {
            if (value) {
                on.draw_within(r, outer);
                panel.move_to({outer.top_left, on.extents()});
            } else {
                off.draw_within(r, outer);
                panel.move_to({outer.top_left, off.extents()});
            }
        }

        felspar::coro::task<void> behaviour() override {
            while (true) {
                co_await panel.clicks.next();
                value = not value;
            }
        }
    };


}