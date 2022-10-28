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


planet::sdl::window::window(
        init const &, const char *const name, std::uint32_t flags)
: pw{SDL_CreateWindow(
        name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, flags)},
  w{640},
  h{480} {
    int ww{}, wh{};
    SDL_GetWindowSize(pw.get(), &ww, &wh);
    w = ww;
    h = wh;
}
