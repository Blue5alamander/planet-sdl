#include <planet/sdl/ui/text.hpp>


planet::sdl::ui::text::text(sdl::font &f, std::string s)
: string{std::move(s)}, font{f}, words{identify_words(string)} {}


auto planet::sdl::ui::text::extents(affine::extents2d const &outer) const
        -> affine::extents2d {
    return outer;
}


void planet::sdl::ui::text::draw_within(renderer &, affine::rectangle2d) {}


std::vector<std::string_view>
        planet::sdl::ui::text::identify_words(std::string &st) {
    std::string_view t{st};
    constexpr std::string_view whitespace{" \n\t"};
    if (t.empty()) { return {}; }
    std::vector<std::string_view> words;
    for (std::size_t start{t.find_first_not_of(whitespace)};
         start != std::string_view::npos;
         start = t.find_first_not_of(whitespace, start)) {
        auto const end = t.find_first_of(whitespace, start);
        if (end == std::string_view::npos) {
            words.push_back(t.substr(start));
            return words;
        } else {
            st[end] = 0;
            words.push_back(t.substr(start, end - start));
            start = end + 1;
        }
    }
    return words;
}
