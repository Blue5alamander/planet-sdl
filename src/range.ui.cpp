#include <planet/sdl/ui/range.hpp>


planet::sdl::ui::range::range(renderer &r, surface bg, surface ctrl)
: background{r, std::move(bg)}, slider{r, std::move(ctrl)} {}


void planet::sdl::ui::range::add_to(planet::panel &parent) {
    parent.add_child(panel);
    response.post(*this, &range::drag);
    visible = true;
}


auto planet::sdl::ui::range::extents(affine::extents2d const &ex) const
        -> affine::extents2d {
    return {ex.width, slider.height()};
}


void planet::sdl::ui::range::draw_within(
        renderer &r, affine::rectangle2d const ex) {
    if (visible) {
        background.draw_within(r, ex);
        slider.draw_within(r, ex);
    }
}


felspar::coro::task<void> planet::sdl::ui::range::drag() { co_return; }
