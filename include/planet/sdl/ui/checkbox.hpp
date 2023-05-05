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

        using constrained_type = planet::ui::widget<renderer>::constrained_type;

        affine::extents2d extents(affine::extents2d const &ex) const {
            auto const on_size = on.extents(ex);
            auto const off_size = off.extents(ex);
            return {std::max(on_size.width, off_size.width),
                    std::max(on_size.height, off_size.height)};
        }

        void draw(renderer &r) {
            if (value) {
                on.draw(r);
            } else {
                off.draw(r);
            }
        }

      private:
        constrained_type do_reflow(constrained_type const &ex) override {
            auto const on_size = on.reflow(ex);
            auto const off_size = off.reflow(ex);
            constrained_type::axis_contrained_type const w{
                    std::max(on_size.width.min(), off_size.width.min()),
                    std::max(on_size.width.value(), off_size.width.value()),
                    std::min(on_size.width.max(), off_size.width.max())};
            constrained_type::axis_contrained_type const h{
                    std::max(on_size.height.min(), off_size.height.min()),
                    std::max(on_size.height.value(), off_size.height.value()),
                    std::min(on_size.height.max(), off_size.height.max())};
            return {w, h};
        }

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
            for (auto clicks = events::identify_clicks(
                         baseplate->mouse_settings, events.mouse.stream());
                 co_await clicks.next();) {
                value = not value;
            }
        }
    };


}
