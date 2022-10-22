#include <planet/sdl/renderer.hpp>
#include <planet/sdl/window.hpp>


planet::sdl::renderer::renderer(window &w) : pr{SDL_CreateRenderer(w, -1, 0)} {}


planet::sdl::renderer::~renderer() {
    if (pr) { SDL_DestroyRenderer(pr); }
}
