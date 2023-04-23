#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    template<typename R>
    class button : public planet::ui::widget<renderer> {
        R press_value;
        felspar::coro::bus<R> &output_to;

      public:
        button(sdl::renderer &r, surface text, felspar::coro::bus<R> &o, R v)
        : press_value{std::move(v)}, output_to{o}, graphic{r, std::move(text)} {}

        affine::extents2d extents(affine::extents2d const &ex) const {
            return graphic.extents(ex);
        }

        texture graphic;

      private:
        void do_draw_within(
                renderer &r, affine::rectangle2d const outer) override {
            panel.move_to({outer.top_left, graphic.extents()});
            graphic.draw_within(r, outer);
        }

        felspar::coro::task<void> behaviour() override {
            for (auto clicks = events::identify_clicks(
                         baseplate->mouse_settings, events.mouse.stream());
                 auto click = co_await clicks.next();) {
                output_to.push(press_value);
            }
        }
    };


}
