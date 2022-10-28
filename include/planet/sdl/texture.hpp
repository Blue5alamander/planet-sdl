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

        SDL_Texture *get() const noexcept { return pt.get(); }

        /// Return the texture extents. The top left co-ordinates will always be
        /// 0, 0
        SDL_Rect extents() const;
    };


}
