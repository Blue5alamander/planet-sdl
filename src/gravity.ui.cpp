#include <planet/sdl/ui/box.hpp>


namespace {
    std::pair<float, float>
            spacing(float left,
                    float right,
                    float const width,
                    bool const at_left,
                    bool const at_right) {
        if (at_left) {
            if (at_right) {
                auto const padding = (right - left - width) / 2;
                left += padding;
                right -= padding;
            } else {
                right = left + width;
            }
        } else if (at_right) {
            left = right - width;
        }
        return {left, right};
    }
}


planet::sdl::ui::extent2d planet::sdl::ui::within(
        gravity const g, extent2d const &o, extent2d const &i) {
    auto [left, right] =
            spacing(o.top_left.x(), o.bottom_right.x(),
                    i.bottom_right.x() - i.top_left.x(), g bitand gravity::left,
                    g bitand gravity::right);
    auto [top, bottom] =
            spacing(o.top_left.y(), o.bottom_right.y(),
                    i.bottom_right.y() - i.top_left.y(), g bitand gravity::top,
                    g bitand gravity::bottom);
    return {{left, top}, {right, bottom}};
}
