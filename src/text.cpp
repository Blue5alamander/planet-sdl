#include <planet/sdl/ui/text.hpp>
#include <planet/sdl/renderer.hpp>

#include <iostream>


planet::sdl::ui::text::text(sdl::font &f, std::string s)
: string{std::move(s)}, font{f}, space{font.measure(" ")} {
    auto const words = identify_words(string);
    for (auto const &w : words) {
        elements.push_back(constrained_type{font.measure(w.data())});
        elements.back().value.word = w;
    }
}


void planet::sdl::ui::text::layout(constrained_type const within) {
    if (elements.laid_out_in != within) {
        elements.laid_out_in = within;
        auto const fit_into = within.extents();
        float top{}, left{}, width{};
        for (auto &element : elements) {
            auto const ex = element.size.extents();
            if (left and left + space.width + ex.width > fit_into.width) {
                width = std::max(width, left);
                top += space.height;
            }
            element.position = {{top, left}, ex};
            if (left) {
                left += space.width + ex.width;
            } else {
                left = ex.width;
            }
        }
        width = std::max(width, left);
        elements.size = {width, width ? top + space.height : 0};
    }
}


auto planet::sdl::ui::text::extents(affine::extents2d const &outer)
        -> affine::extents2d {
    layout(constrained_type{outer});
    return elements.size.value();
}


void planet::sdl::ui::text::draw_within(
        renderer &r, affine::rectangle2d const within) {
    extents(within.extents);
    for (auto &element : elements) {
        if (not element.value.texture) {
            element.value.texture = {r, font.render(element.value.word.data())};
        }
        if (element.position) {
            r.copy(*element.value.texture,
                   static_cast<int>(element.position->left()),
                   static_cast<int>(element.position->top()));
        }
    }
}


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
