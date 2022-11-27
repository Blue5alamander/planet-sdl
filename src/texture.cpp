#include <planet/sdl/renderer.hpp>
#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>


planet::sdl::texture::texture(renderer &r, surface const &s)
: pt{SDL_CreateTextureFromSurface(r.get(), s.get())}, size{s.extents()} {}


void planet::sdl::texture::draw_within(
        planet::sdl::renderer &r, planet::affine::extent2d const &e) const {
    SDL_Rect location = {
            int(e.top_left.x()), int(e.top_left.y()), int(e.width()),
            int(e.height())};
    drawing_worked(SDL_RenderCopy(r.get(), pt.get(), nullptr, &location));
}
