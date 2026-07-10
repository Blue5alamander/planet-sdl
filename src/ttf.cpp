#include <planet/sdl/ttf.hpp>
#include <planet/ui/scale.hpp>

#include <felspar/exceptions/runtime_error.hpp>

#include <source_location>
#include <string>


namespace {


    /// Throw if an SDL API call reports failure
    int check_worked(
            int const e,
            std::source_location const &loc = std::source_location::current()) {
        if (e < 0) [[unlikely]] {
            throw felspar::stdexcept::runtime_error{
                    std::string{"SDL TTF API failed "} + SDL_GetError(), loc};
        } else {
            return e;
        }
    }


}


/// ## `planet::sdl::ttf`


planet::sdl::ttf::ttf(init &) { TTF_Init(); }


planet::sdl::ttf::~ttf() { TTF_Quit(); }


/// ## `planet::sdl::font`


planet::sdl::font::font(
        asset_manager const &am,
        char const *const filename,
        std::size_t const pixels,
        SDL_Color const c,
        std::source_location const &loc)
: font_data{am, filename, loc},
  pf{TTF_OpenFontRW(font_data.get(), false, pixels)},
  colour{c},
  space{measure(" ")},
  em{measure("m")} {
    if (not pf.get()) {
        throw felspar::stdexcept::runtime_error{
                "TTF_OpenFontRW returned nullptr"};
    }
}


planet::affine::extents2d planet::sdl::font::measure(char const *const t) const {
    int w{}, h{};
    check_worked(TTF_SizeUTF8(pf.get(), t, &w, &h));
    return {static_cast<float>(w), static_cast<float>(h)};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text, SDL_Color c, ui::scale const fit) const {
    return {TTF_RenderUTF8_Blended(pf.get(), text, c), fit};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text,
        SDL_Color const c,
        std::uint32_t const width,
        ui::scale const fit) const {
    return {TTF_RenderUTF8_Blended_Wrapped(pf.get(), text, c, width), fit};
}
