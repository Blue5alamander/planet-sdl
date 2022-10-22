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
        renderer &re,
        std::uint8_t const r,
        std::uint8_t const g,
        std::uint8_t const b)
: rend{re} {
    colour(r, g, b);
    SDL_RenderClear(rend);
}


planet::sdl::drawframe::~drawframe() { SDL_RenderPresent(rend); }


void planet::sdl::drawframe::colour(
        std::uint8_t const r, std::uint8_t const g, std::uint8_t const b) const {
    SDL_SetRenderDrawColor(rend, r, g, b, 255);
}


void planet::sdl::drawframe::line(
        std::size_t const x1,
        std::size_t const y1,
        std::size_t const x2,
        std::size_t const y2) const {
    SDL_RenderDrawLine(rend, x1, y1, x2, y2);
}
