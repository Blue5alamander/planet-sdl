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
: rend{re}, w{rend.win.width()}, h{rend.win.height()} {
    colour(r, g, b);
    SDL_RenderClear(rend);
    viewport.reflect_y().scale(30.0f).translate({w / 2.0f, h / 2.0f});
}


planet::sdl::drawframe::~drawframe() { SDL_RenderPresent(rend); }


void planet::sdl::drawframe::colour(
        std::uint8_t const r, std::uint8_t const g, std::uint8_t const b) const {
    SDL_SetRenderDrawColor(rend, r, g, b, SDL_ALPHA_OPAQUE);
}


void planet::sdl::drawframe::line(
        std::size_t const x1,
        std::size_t const y1,
        std::size_t const x2,
        std::size_t const y2) const {
    SDL_RenderDrawLine(rend, x1, y1, x2, y2);
}
