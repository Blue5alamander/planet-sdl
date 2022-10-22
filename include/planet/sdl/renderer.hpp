#pragma once


#include <SDL.h>


namespace planet::sdl {


    class window;


    class renderer {
        SDL_Renderer *pr = nullptr;

      public:
        renderer(window &);
        ~renderer();

        operator SDL_Renderer *() const noexcept { return pr; }
    };


}
