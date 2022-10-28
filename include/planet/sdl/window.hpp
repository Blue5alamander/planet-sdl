#pragma once


#include <planet/sdl/handle.hpp>

#include <SDL.h>


namespace planet::sdl {


    class init;


    class window final {
        handle<SDL_Window, SDL_DestroyWindow> pw;
        std::size_t w, h;

      public:
        window(init const &,
               const char *name,
               std::size_t width,
               std::size_t height);

        operator SDL_Window *() const noexcept { return pw; }

        std::size_t width() const noexcept { return w; }
        std::size_t height() const noexcept { return h; }
    };


}
