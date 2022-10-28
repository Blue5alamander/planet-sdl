#include <planet/sdl/renderer.hpp>
#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>


planet::sdl::texture::texture(renderer &r, surface const &s)
: pt{SDL_CreateTextureFromSurface(r, s)} {}


SDL_Rect planet::sdl::texture::extents() const {
    int w{}, h{};
    SDL_QueryTexture(pt, nullptr, nullptr, &w, &h);
    return {0, 0, w, h};
}