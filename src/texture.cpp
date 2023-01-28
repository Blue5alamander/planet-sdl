#include <planet/sdl/renderer.hpp>
#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>


planet::sdl::texture::texture(renderer &r, surface const &s)
: pt{SDL_CreateTextureFromSurface(r.get(), s.get())},
  size{s.extents()},
  fit{s.fit} {}


void planet::sdl::texture::draw_within(
        planet::sdl::renderer &r, planet::affine::rectangle2d const &e) const {
    auto const ex = extents(e.extents);
    SDL_Rect location = {
            int(e.top_left.x()), int(e.top_left.y()), int(ex.width),
            int(ex.height)};
    drawing_worked(SDL_RenderCopy(r.get(), pt.get(), nullptr, &location));
}


planet::affine::extents2d planet::sdl::texture::extents(
        affine::extents2d const bounds) const noexcept {
    return ui::scaling(size, bounds, fit);
}
