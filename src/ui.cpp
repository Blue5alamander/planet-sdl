#include <planet/sdl/renderer.hpp>
#include <planet/sdl/ui/range.hpp>
#include <planet/sdl/ui/text.hpp>
#include <planet/ui/baseplate.hpp>

#include <planet/ostream.hpp>
#include <iostream>


/// ## `planet::sdl::ui::draggable`


static_assert(planet::ui::reflowable<planet::sdl::ui::draggable>);


planet::sdl::ui::draggable::draggable(
        renderer &r, surface ctrl, affine::point2d const &o)
: hotspot{r, std::move(ctrl)}, offset{o} {}


auto planet::sdl::ui::draggable::reflow(constrained_type const &constraint)
        -> constrained_type {
    return hotspot.reflow(constraint);
}


void planet::sdl::ui::draggable::do_draw_within(
        renderer &r, affine::rectangle2d const ex) {
    hotspot.draw_within(r, {ex.top_left + offset, ex.extents});
    panel.move_to({ex.top_left, hotspot.extents(ex.extents)});
}


felspar::coro::task<void> planet::sdl::ui::draggable::behaviour() {
    std::cout << "Starting draggable behaviour\n";
    std::optional<affine::point2d> base, start;
    while (true) {
        auto event = co_await events.mouse.next();
        if (event.button == events::button::left) {
            if (event.action == events::action::down) {
                start = offset;
                base = panel.outof(event.location);
                baseplate->hard_focus_on(this);
            } else if (base and start and event.action == events::action::held) {
                auto const locnow = panel.outof(event.location);
                offset = *start + locnow - *base;
            } else if (event.action == events::action::up) {
                base = {};
                start = {};
                baseplate->hard_focus_off(this);
            }
        }
    }
}


/// ## `planet::sdl::ui::range`


static_assert(planet::ui::reflowable<planet::sdl::ui::range>);


planet::sdl::ui::range::range(renderer &r, surface bg, surface ctrl)
: background{r, std::move(bg)}, slider{r, std::move(ctrl), {{}, {}}} {}


void planet::sdl::ui::range::add_to(
        planet::ui::baseplate<renderer> &bp,
        planet::ui::panel &parent,
        float const z_layer) {
    planet::ui::widget<renderer>::add_to(bp, parent, z_layer);
    slider.add_to(bp, parent, z_layer + 1);
    std::cout << "Added range to baseplate\n";
}


void planet::sdl::ui::range::do_draw_within(
        renderer &r, affine::rectangle2d const ex) {
    background.draw_within(r, ex);
    panel.move_to({ex.top_left, background.extents(ex.extents)});
    slider.draw_within(r, ex);
}


auto planet::sdl::ui::range::reflow(constrained_type const &constraint)
        -> constrained_type {
    slider.reflow(constraint);
    return background.reflow(constraint);
}


felspar::coro::task<void> planet::sdl::ui::range::behaviour() { co_return; }


/// ## `planet::sdl::ui::text`


planet::sdl::ui::text::text(sdl::font &f, std::string_view const s)
: font{f}, space{font.measure(" ")} {
    auto const words = identify_words(s);
    for (auto const &w : words) {
        std::string word{w};
        elements.emplace_back(constrained_type{font.measure(word.c_str())});
        elements.back().value.word = std::move(word);
    }
}


void planet::sdl::ui::text::reflow(constrained_type const within) {
    if (not elements.laid_out_in
        or not elements.laid_out_in->is_at_least_as_constrained_as(within)) {
        elements.laid_out_in = within;
        auto const fit_into = within.extents();
        float top{}, left{}, width{};
        for (auto &element : elements) {
            auto const ex = element.size.extents();
            if (left and left + ex.width > fit_into.width) {
                width = std::max(width, left);
                left = {};
                top += space.height;
            }
            element.position = {{left, top}, ex};
            left += space.width + ex.width;
        }
        width = std::max(width, left);
        elements.extents = {width, width ? top + space.height : 0};
    }
}


auto planet::sdl::ui::text::extents(affine::extents2d const &outer)
        -> affine::extents2d {
    reflow(constrained_type{outer});
    return elements.extents.value();
}


void planet::sdl::ui::text::draw_within(
        renderer &r, affine::rectangle2d const within) {
    for (auto &element : elements) {
        if (not element.value.texture) {
            element.value.texture = {
                    r, font.render(element.value.word.c_str())};
        }
        if (element.position) {
            auto const p = element.position->top_left + within.top_left;
            r.copy(*element.value.texture, static_cast<int>(p.x()),
                   static_cast<int>(p.y()));
        }
    }
}


std::vector<std::string_view>
        planet::sdl::ui::text::identify_words(std::string_view const t) {
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
            words.push_back(t.substr(start, end - start));
            start = end + 1;
        }
    }
    return words;
}
