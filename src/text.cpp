#include <planet/sdl/ui/text.hpp>


planet::sdl::ui::text::text(sdl::font &f, std::string s)
: string{std::move(s)}, font{f} {}


auto planet::sdl::ui::text::extents(affine::extents2d const &outer) const
        -> affine::extents2d {
    return outer;
}


void planet::sdl::ui::text::draw_within(renderer &, affine::rectangle2d) {}


std::vector<std::string_view>
        planet::sdl::ui::text::identify_words(std::string_view) {
    return {};
}
