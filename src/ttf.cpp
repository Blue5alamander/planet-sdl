#include <planet/sdl/ttf.hpp>
#include <planet/ui/scale.hpp>

#include <felspar/exceptions/runtime_error.hpp>

#include <source_location>
#include <string>


namespace {


    /// Throw if an SDL TTF API call reports failure
    /**
     * SDL3's TTF calls (sizing, rendering and `TTF_Init`) return `bool` and
     * report failure as `false`.
     */
    void check_worked(
            bool const ok,
            std::source_location const &loc = std::source_location::current()) {
        if (not ok) [[unlikely]] {
            throw felspar::stdexcept::runtime_error{
                    std::string{"SDL TTF API failed "} + SDL_GetError(), loc};
        }
    }


}


/// ## `planet::sdl::ttf`


planet::sdl::ttf::ttf(init &) { check_worked(TTF_Init()); }


planet::sdl::ttf::~ttf() { TTF_Quit(); }


/// ## `planet::sdl::font`


planet::sdl::font::font(
        asset_manager const &am,
        char const *const filename,
        std::size_t const pixels,
        SDL_Color const c,
        std::source_location const &loc)
: font_data{am, filename, loc},
  pf{TTF_OpenFontIO(font_data.get(), false, static_cast<float>(pixels))},
  colour{c},
  space{measure(" ")},
  em{measure("m")} {
    if (not pf.get()) {
        throw felspar::stdexcept::runtime_error{
                "TTF_OpenFontIO returned nullptr"};
    }
}


planet::affine::extents2d planet::sdl::font::measure(char const *const t) const {
    int w{}, h{};
    check_worked(TTF_GetStringSize(pf.get(), t, 0, &w, &h));
    return {static_cast<float>(w), static_cast<float>(h)};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text, SDL_Color c, ui::scale const fit) const {
    return {TTF_RenderText_Blended(pf.get(), text, 0, c), fit};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text,
        SDL_Color const c,
        std::uint32_t const width,
        ui::scale const fit) const {
    return {TTF_RenderText_Blended_Wrapped(pf.get(), text, 0, c, width), fit};
}
