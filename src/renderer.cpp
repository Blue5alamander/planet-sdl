#include <planet/sdl/renderer.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/sdl/window.hpp>


/**
 * ## `planet::sdl::renderer`
 */


planet::sdl::renderer::renderer(window &w)
: win{w}, pr{SDL_CreateRenderer(win.get(), -1, 0)} {}


void planet::sdl::renderer::present() { SDL_RenderPresent(pr.get()); }


void planet::sdl::renderer::colour(
        std::uint8_t const r, std::uint8_t const g, std::uint8_t const b) const {
    SDL_SetRenderDrawColor(pr.get(), r, g, b, SDL_ALPHA_OPAQUE);
}


void planet::sdl::renderer::line(
        std::size_t const x1,
        std::size_t const y1,
        std::size_t const x2,
        std::size_t const y2) const {
    SDL_RenderDrawLine(pr.get(), x1, y1, x2, y2);
}


void planet::sdl::renderer::lines(std::span<SDL_Point> pts) const {
    SDL_RenderDrawLines(pr.get(), pts.data(), pts.size());
}


void planet::sdl::renderer::copy(
        texture const &t, std::size_t const x, std::size_t const y) {
    auto location = t.extents();
    location.x = x;
    location.y = y;
    SDL_RenderCopy(pr.get(), t.get(), nullptr, &location);
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
    rend.colour(r, g, b);
    SDL_RenderClear(rend.get());
}


void planet::sdl::drawframe::line(
        affine::point2d const cp1, affine::point2d const cp2) const {
    auto p1 = viewport.into(cp1);
    auto p2 = viewport.into(cp2);
    rend.line(p1.x(), p1.y(), p2.x(), p2.y());
}
