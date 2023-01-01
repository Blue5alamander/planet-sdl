#include <planet/sdl/init.hpp>
#include <planet/sdl/window.hpp>

#include <felspar/exceptions.hpp>

#include <iostream>


using namespace std::literals;


namespace {
    felspar::coro::stream<planet::events::mouse>
            inputs(felspar::coro::bus<planet::events::mouse> &inputs) {
        while (true) { co_yield co_await inputs.next(); }
    }
    felspar::coro::task<void> mouse_clicks(
            felspar::coro::stream<planet::events::click> inputs,
            felspar::coro::bus<planet::events::click> &outputs) {
        while (auto m = co_await inputs.next()) { outputs.push(*m); }
    }
}


planet::sdl::window::window(
        init &,
        const char *const name,
        int const posx,
        int const posy,
        int const width,
        int const height,
        std::uint32_t const flags)
  pw{SDL_CreateWindow(
          name,
          SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED,
          width,
          height,
          flags)},
  renderer{*this},
  size{float(width), float(height)} {
    if (not pw.get()) {
        throw felspar::stdexcept::runtime_error{"SDL_CreateWindow failed"};
    }
    std::cout << "Window created\n";
    int ww{}, wh{};
    SDL_GL_GetDrawableSize(pw.get(), &ww, &wh);
    size = {float(ww), float(wh)};

    processes.post(
            mouse_clicks,
            planet::events::identify_clicks(mouse_settings, inputs(raw_mouse)),
            std::ref(renderer.screen.clicks));
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
