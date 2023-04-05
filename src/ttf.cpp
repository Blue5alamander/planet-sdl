#include <planet/sdl/renderer.hpp>
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
                "TTF_OpenFontRW returned nullptr"};
    }
}


planet::affine::extents2d planet::sdl::font::measure(char const *const t) {
    int w{}, h{};
    drawing_worked(TTF_SizeUTF8(pf.get(), t, &w, &h));
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
