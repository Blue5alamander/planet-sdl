#include <planet/sdl/renderer.hpp>
#include <planet/sdl/window.hpp>


/**
 * ## `planet::sdl::renderer`
 */


planet::sdl::renderer::renderer(window &w)
: win{w}, pr{SDL_CreateRenderer(win, -1, 0)} {}


planet::sdl::renderer::~renderer() {
    if (pr) { SDL_DestroyRenderer(pr); }
}


/**
 * ## `planet::sdl::drawframe`
 */


planet::sdl::drawframe::drawframe(
        renderer &re, std::uint8_t r, std::uint8_t g, std::uint8_t b)
: rend{re} {
    SDL_SetRenderDrawColor(rend, r, g, b, 255);
    SDL_RenderClear(rend);
}


planet::sdl::drawframe::~drawframe() { SDL_RenderPresent(rend); }
