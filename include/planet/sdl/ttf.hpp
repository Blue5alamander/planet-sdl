#pragma once


#include <planet/asset_manager.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/sdl/rw_ops.hpp>
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
        rw_ops_const_memory font_data;
        handle<TTF_Font, TTF_CloseFont> pf;
        SDL_Color colour;

      public:
        font(asset_manager const &,
             char const *filename,
             std::size_t pixel_height,
             SDL_Color = {255, 255, 200, 255},
             felspar::source_location const & =
                     felspar::source_location::current());

        TTF_Font *get() const noexcept { return pf.get(); }

        /// Render text to a single line
        surface render(char const *text) const { return render(text, colour); }
        surface render(char const *text, SDL_Color) const;

        /// Wrap text if  it is too wide
        surface render(char const *text, std::uint32_t const width) const {
            return render(text, colour, width);
        }
        surface render(char const *text, SDL_Color, std::uint32_t width) const;
    };


}
