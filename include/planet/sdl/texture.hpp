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

        /// Return the texture extents. The top left co-ordinates will always be
        /// 0, 0
        SDL_Rect extents() const;
    };


}
