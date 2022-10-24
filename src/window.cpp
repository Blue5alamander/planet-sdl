#include <planet/sdl/window.hpp>


planet::sdl::window::window(
        init const &,
        const char *const name,
        std::size_t const width,
        std::size_t const height)
: pw{SDL_CreateWindow(
        name,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        0)},
  w{width},
  h{height} {}


planet::sdl::window::~window() {
    if (pw) { SDL_DestroyWindow(pw); }
}
