#pragma once


#include <SDL.h>


namespace planet::sdl {


    class init;


    class window final {
        SDL_Window *pw = nullptr;

      public:
        window(init const &,
               const char *name,
               std::size_t width,
               std::size_t height);
        ~window();

        operator SDL_Window *() const noexcept { return pw; }
    };


}
