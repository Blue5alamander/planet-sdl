#pragma once


#include <planet/sdl/handle.hpp>
#include <planet/sdl/surface.hpp>

#include <SDL_ttf.h>


namespace planet::sdl {


    class init;


    /// Create one of these for access to the TTF rendering parts of SDL2
    class ttf {
      public:
        ttf(init &);
        ~ttf();
    };


    class font {
        handle<TTF_Font, TTF_CloseFont> pf;

      public:
        font(char const *filename, std::size_t pixels);

        TTF_Font *get() const noexcept { return pf.get(); }

        surface render(char const *text, SDL_Color const &color) const;
    };


}
