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
        char const *const filename, std::size_t const pixels, SDL_Color const c)
: pf{TTF_OpenFont(filename, pixels)}, colour{c} {}


planet::sdl::surface planet::sdl::font::render(char const *text) const {
    return {TTF_RenderText_Solid(pf.get(), text, colour)};
}
