#pragma once


#include <planet/sdl/handle.hpp>

#include <SDL.h>


namespace planet::sdl {


    class surface {
      public:
        using handle_type = handle<SDL_Surface, SDL_FreeSurface>;

        surface(handle_type h) : ps{std::move(h)} {}

        operator SDL_Surface *() const noexcept { return ps; }

      private:
        handle_type ps;
    };


}