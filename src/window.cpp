#include <planet/log.hpp>
#include <planet/sdl/init.hpp>
#include <planet/sdl/window.hpp>

#include <felspar/exceptions.hpp>


using namespace std::literals;


planet::sdl::window::window(
        init &,
        const char *const name,
        int const posx,
        int const posy,
        int const width,
        int const height,
        std::uint32_t const flags)
: pw{SDL_CreateWindow(name, posx, posy, width, height, flags)},
  size{float(width), float(height)},
  renderer{*this} {
    if (not pw.get()) {
        throw felspar::stdexcept::runtime_error{"SDL_CreateWindow failed"};
    }
    int ww{}, wh{};
    SDL_GL_GetDrawableSize(pw.get(), &ww, &wh);
    size = {float(ww), float(wh)};
    planet::log::info("Window created", ww, wh);
}


planet::sdl::window::window(
        init &s,
        const char *const name,
        std::size_t const width,
        std::size_t const height,
        std::uint32_t const flags)
: window{s,
         name,
         SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED,
         int(width),
         int(height),
         flags} {}


planet::sdl::window::window(
        init &s, const char *const name, std::uint32_t const flags)
: window{s,   name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640,
         480, flags} {}
