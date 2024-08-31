#include <planet/sdl/renderer.hpp>
#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>


planet::sdl::texture::texture(renderer &r, surface const &s)
: reflowable{"planet::sdl::texture"},
  pt{SDL_CreateTextureFromSurface(r.get(), s.get())},
  size{s.extents()},
  rp{&r},
  fit{s.fit} {}
planet::sdl::texture::texture(
        std::string_view const n, renderer &r, surface const &s)
: reflowable{n},
  pt{SDL_CreateTextureFromSurface(r.get(), s.get())},
  size{s.extents()},
  rp{&r},
  fit{s.fit} {}


void planet::sdl::texture::draw_within(
        planet::sdl::renderer &, planet::affine::rectangle2d const &e) const {
    auto const ex = extents(e.extents);
    SDL_Rect location = {
            int(e.top_left.x()), int(e.top_left.y()), int(ex.width),
            int(ex.height)};
    drawing_worked(SDL_RenderCopy(rp->get(), pt.get(), nullptr, &location));
}
void planet::sdl::texture::draw() {
    auto const &p = position();
    SDL_Rect location = {
            int(p.top_left.x()), int(p.top_left.y()), int(p.extents.width),
            int(p.extents.height)};
    drawing_worked(SDL_RenderCopy(rp->get(), pt.get(), nullptr, &location));
}


auto planet::sdl::texture::do_reflow(
        reflow_parameters const &,
        constrained_type const &constraint) -> constrained_type {
    return ui::scaling(size, constraint, fit);
}


planet::affine::extents2d planet::sdl::texture::extents(
        affine::extents2d const bounds) const noexcept {
    return ui::scaling(size, bounds, fit);
}
