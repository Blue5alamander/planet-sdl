#include <planet/sdl/ttf.hpp>


/**
 * `planet::sdl::ttf`
 */


planet::sdl::ttf::ttf(init &) { TTF_Init(); }


planet::sdl::ttf::~ttf() { TTF_Quit(); }


/**
 * `planet::sdl::font`
 */


planet::sdl::font::font(
        asset_manager const &am,
        char const *const filename,
        std::size_t const pixels,
        SDL_Color const c,
        felspar::source_location const &loc)
: pf{TTF_OpenFont(am.find_path(filename, loc).c_str(), pixels)}, colour{c} {}


planet::sdl::surface planet::sdl::font::render(char const *text) const {
    return {TTF_RenderText_Solid(pf.get(), text, colour)};
}
planet::sdl::surface
        planet::sdl::font::render(char const *text, SDL_Color c) const {
    return {TTF_RenderText_Solid(pf.get(), text, c)};
}
