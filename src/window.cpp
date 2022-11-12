#include <planet/sdl/init.hpp>
#include <planet/sdl/window.hpp>


using namespace std::literals;


planet::sdl::window::window(
        init &s,
        const char *const name,
        std::size_t const width,
        std::size_t const height)
: sdl{s},
  pw{SDL_CreateWindow(
          name,
          SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED,
          width,
          height,
          0)},
  w{width},
  h{height} {}


planet::sdl::window::window(init &s, const char *const name, std::uint32_t flags)
: sdl{s},
  pw{SDL_CreateWindow(
          name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, flags)},
  w{640},
  h{480} {
    int ww{}, wh{};
    SDL_GL_GetDrawableSize(pw.get(), &ww, &wh);
    w = ww;
    h = wh;
}
