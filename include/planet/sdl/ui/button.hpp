#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/widget.hpp>


namespace planet::sdl::ui {


    template<typename R>
    class button : public planet::ui::widget<renderer> {
        using superclass = planet::ui::widget<renderer>;
        R press_value;
        felspar::coro::bus<R> &output_to;

      public:
        button(sdl::renderer &r, surface text, felspar::coro::bus<R> &o, R v)
        : superclass{"planet::sdl::ui::button"},
          press_value{std::move(v)},
          output_to{o},
          graphic{"planet::sdl::ui::button.graphic", r, std::move(text)} {}
        button(std::string_view const n,
               sdl::renderer &r,
               surface text,
               felspar::coro::bus<R> &o,
               R v)
        : superclass{n},
          press_value{std::move(v)},
          output_to{o},
          graphic{std::string{n} + ".graphic", r, std::move(text)} {}

        affine::extents2d extents(affine::extents2d const &ex) const {
            return graphic.extents(ex);
        }

        texture graphic;

      private:
        constrained_type do_reflow(constrained_type const &ex) override {
            return graphic.reflow(ex);
        }
        void do_move_sub_elements(affine::rectangle2d const &r) override {
            graphic.move_to(r);
        }

        void do_draw_within(
                renderer &r, affine::rectangle2d const outer) override {
            panel.move_to({outer.top_left, graphic.extents()});
            graphic.draw_within(r, outer);
        }
        void do_draw(renderer &r) override { graphic.draw(r); }

        felspar::coro::task<void> behaviour() override {
            for (auto clicks = events::identify_clicks(
                         baseplate->mouse_settings, events.mouse.stream());
                 auto click = co_await clicks.next();) {
                output_to.push(press_value);
            }
        }
    };


}
