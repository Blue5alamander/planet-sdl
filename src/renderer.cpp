#include <planet/sdl/renderer.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/sdl/window.hpp>

#include <iostream>


/**
 * ## `planet::sdl::renderer`
 */


planet::sdl::renderer::renderer(window &w)
: win{w}, pr{SDL_CreateRenderer(win.get(), -1, 0)} {
    if (not pr.get()) [[unlikely]] {
        throw felspar::stdexcept::runtime_error{
                std::string{"SDL_CreateRenderer failed: "} + SDL_GetError()};
    }
    std::cout << "Renderer created\n";
}


void planet::sdl::renderer::clear() {
    ++current_frame.number;
    drawing_worked(SDL_RenderClear(pr.get()));
}


planet::sdl::renderer::frame planet::sdl::renderer::present() {
    SDL_RenderPresent(pr.get());
    return current_frame;
}


void planet::sdl::renderer::colour(
        std::uint8_t const r, std::uint8_t const g, std::uint8_t const b) const {
    drawing_worked(SDL_SetRenderDrawColor(pr.get(), r, g, b, SDL_ALPHA_OPAQUE));
}
void planet::sdl::renderer::colour(SDL_Color const &c) const {
    drawing_worked(
            SDL_SetRenderDrawColor(pr.get(), c.r, c.g, c.b, SDL_ALPHA_OPAQUE));
}


void planet::sdl::renderer::line(
        int const x1, int const y1, int const x2, int const y2) const {
    drawing_worked(SDL_RenderDrawLine(pr.get(), x1, y1, x2, y2));
}


void planet::sdl::renderer::lines(std::span<SDL_Point> pts) const {
    drawing_worked(SDL_RenderDrawLines(pr.get(), pts.data(), pts.size()));
}


void planet::sdl::renderer::copy(texture const &t, int const x, int const y) {
    SDL_Rect location = {int(x), int(y), int(t.zwidth()), int(t.zheight())};
    drawing_worked(SDL_RenderCopy(pr.get(), t.get(), nullptr, &location));
}
void planet::sdl::renderer::copy(
        texture const &t, int const x, int const y, int const w, int const h) {
    SDL_Rect location = {int(x), int(y), int(w), int(h)};
    drawing_worked(SDL_RenderCopy(pr.get(), t.get(), nullptr, &location));
}
