#include <planet/sdl/renderer.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/sdl/window.hpp>


/**
 * ## `planet::sdl::renderer`
 */


planet::sdl::renderer::renderer(window &w)
: win{w}, pr{SDL_CreateRenderer(win.get(), -1, 0)} {}


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
    SDL_RenderClear(rend.get());
}


planet::sdl::drawframe::~drawframe() { SDL_RenderPresent(rend.get()); }


void planet::sdl::drawframe::colour(
        std::uint8_t const r, std::uint8_t const g, std::uint8_t const b) const {
    SDL_SetRenderDrawColor(rend.get(), r, g, b, SDL_ALPHA_OPAQUE);
}


void planet::sdl::drawframe::line(
        std::size_t const x1,
        std::size_t const y1,
        std::size_t const x2,
        std::size_t const y2) const {
    SDL_RenderDrawLine(rend.get(), x1, y1, x2, y2);
}


void planet::sdl::drawframe::lines(std::span<SDL_Point> pts) const {
    SDL_RenderDrawLines(rend.get(), pts.data(), pts.size());
}


void planet::sdl::drawframe::copy(
        texture const &t, std::size_t const x, std::size_t const y) {
    auto location = t.extents();
    location.x = x;
    location.y = y;
    SDL_RenderCopy(rend.get(), t.get(), nullptr, &location);
}
