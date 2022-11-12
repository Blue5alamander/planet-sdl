#pragma once


#include <planet/sdl/handle.hpp>

#include <SDL.h>

#include <cstdint>


namespace planet::sdl {


    class init;


    class window final {
        init &sdl;
        handle<SDL_Window, SDL_DestroyWindow> pw;
        std::size_t w, h;

      public:
        window(init &, const char *name, std::size_t width, std::size_t height);
        window(init &, const char *name, std::uint32_t flags);


        SDL_Window *get() const noexcept { return pw.get(); }

        /// Current inner window size
        std::size_t width() const noexcept { return w; }
        std::size_t height() const noexcept { return h; }
    };


}
