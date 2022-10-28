#pragma once


#include <planet/sdl/handle.hpp>

#include <SDL.h>


namespace planet::sdl {


    class renderer;
    class surface;


    class texture {
        handle<SDL_Texture, SDL_DestroyTexture> pt;

      public:
        texture(renderer &, surface const &);

        operator SDL_Texture *() const noexcept { return pt; }
    };


}
