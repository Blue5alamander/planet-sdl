#pragma once


namespace planet::sdl::ui {


    template<typename R>
    class button {
      public:
        planet::panel panel;
        texture graphic;
        bool visible = false;

        R press_value;
        felspar::coro::bus<R> &output_to;

        button(sdl::renderer &r, surface text, felspar::coro::bus<R> &o, R v)
        : graphic{r, std::move(text)}, press_value{std::move(v)}, output_to{o} {}

        void add_to(planet::panel &parent) {
            parent.add_child(panel);
            response.post(*this, &button::button_response);
            visible = true;
        }

        affine::extents2d extents(affine::extents2d const &ex) const {
            return graphic.extents(ex);
        }
        void draw_within(renderer &r, affine::rectangle2d const outer) {
            if (visible) {
                graphic.draw_within(r, outer);
                panel.move_to({outer.top_left, graphic.extents()});
            }
        }

      private:
        felspar::coro::eager<> response;
        felspar::coro::task<void> button_response() {
            while (true) {
                co_await panel.clicks.next();
                output_to.push(press_value);
            }
        }
    };


}
