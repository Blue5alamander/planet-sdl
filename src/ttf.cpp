#include <planet/sdl/ttf.hpp>
#include <planet/ui/scale.hpp>


/// ## `planet::sdl::ttf`


planet::sdl::ttf::ttf(init &) { TTF_Init(); }


planet::sdl::ttf::~ttf() { TTF_Quit(); }


/// ## `planet::sdl::font`


planet::sdl::font::font(
        asset_manager const &am,
        char const *const filename,
        std::size_t const pixels,
        SDL_Color const c,
        felspar::source_location const &loc)
: font_data{am, filename, loc},
  pf{TTF_OpenFontRW(font_data.get(), false, pixels)},
  colour{c} {
    if (not pf.get()) {
        throw felspar::stdexcept::runtime_error{
                "TTF_OpenFontRW return nullptr"};
    }
}


planet::sdl::surface
        planet::sdl::font::render(char const *text, SDL_Color c) const {
    return {TTF_RenderUTF8_Blended(pf.get(), text, c), ui::scale::never};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text, SDL_Color const c, std::uint32_t const width) const {
    return {TTF_RenderUTF8_Blended_Wrapped(pf.get(), text, c, width),
            ui::scale::never};
}
