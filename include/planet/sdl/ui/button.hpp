#pragma once


namespace planet::sdl::ui {


    template<typename R>
    class button {
      public:
        sdl::panel panel;
        texture graphic;
        bool visible = false;

        R press_value;
        felspar::coro::bus<R> &output_to;

        button(sdl::renderer &r, surface text, felspar::coro::bus<R> &o, R v)
        : panel{r},
          graphic{r, std::move(text)},
          press_value{std::move(v)},
          output_to{o} {}

        void
                add_to(planet::sdl::panel &parent,
                       planet::affine::point2d const centre) {
            auto const sz = graphic.extents();
            affine::point2d const half = {sz.width / 2.0f, sz.height / 2.0f};
            parent.add_child(panel, centre - half, centre + half);
            response.post(*this, &button::button_response);
            visible = true;
        }
        void draw() const {
            if (visible) { panel.copy(graphic, {0, 0}); }
        }

        affine::extents2d extents(affine::extents2d const &ex) const {
            return graphic.extents(ex);
        }
        void draw_within(renderer &r, affine::rectangle const outer) const {
            if (visible) { graphic.draw_within(r, outer); }
        }

      private:
        felspar::coro::eager<> response;
        felspar::coro::task<void> button_response() {
            while (true) {
                co_await panel.mouse_click.next();
                output_to.push(press_value);
            }
        }
    };


}
