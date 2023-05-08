#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    class checkbox final : public planet::ui::widget<renderer> {
        using superclass = planet::ui::widget<renderer>;
        texture on, off;
        bool &value;

      public:
        explicit checkbox(renderer &r, surface on, surface off, bool &v)
        : superclass{"planet::sdl::ui::checkbox"},
          on{r, std::move(on)},
          off{r, std::move(off)},
          value{v} {}
        explicit checkbox(
                std::string_view const n,
                renderer &r,
                surface on,
                surface off,
                bool &v)
        : superclass{n}, on{r, std::move(on)}, off{r, std::move(off)}, value{v} {}

        using constrained_type = planet::ui::widget<renderer>::constrained_type;

        affine::extents2d extents(affine::extents2d const &ex) const {
            auto const on_size = on.extents(ex);
            auto const off_size = off.extents(ex);
            return {std::max(on_size.width, off_size.width),
                    std::max(on_size.height, off_size.height)};
        }

      private:
        constrained_type do_reflow(constrained_type const &ex) override {
            auto const on_size = on.reflow(ex);
            auto const off_size = off.reflow(ex);
            constrained_type::axis_constrained_type const w{
                    std::max(on_size.width.min(), off_size.width.min()),
                    std::max(on_size.width.value(), off_size.width.value()),
                    std::min(on_size.width.max(), off_size.width.max())};
            constrained_type::axis_constrained_type const h{
                    std::max(on_size.height.min(), off_size.height.min()),
                    std::max(on_size.height.value(), off_size.height.value()),
                    std::min(on_size.height.max(), off_size.height.max())};
            return {w, h};
        }
        void do_move_sub_elements(affine::rectangle2d const &r) override {
            on.move_to(r);
            off.move_to(r);
        }

        void do_draw(renderer &r) override {
            if (value) {
                on.draw(r);
            } else {
                off.draw(r);
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
