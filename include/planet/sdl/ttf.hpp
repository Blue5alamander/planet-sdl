#pragma once


#include <planet/affine/extents2d.hpp>
#include <planet/asset_manager.hpp>
#include <planet/sdl/forward.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/sdl/rw_ops.hpp>
#include <planet/sdl/surface.hpp>

#include <SDL3_ttf/SDL_ttf.h>


namespace planet::sdl {


    /// ## Initialise TTF
    /// Create one of these for access to the TTF rendering parts of SDL2
    class ttf {
      public:
        ttf(init &);
        ~ttf();
    };


    /// ## TTF Font
    class font {
        rw_ops_const_memory font_data;
        handle<TTF_Font, TTF_CloseFont> pf;
        SDL_Color colour;

      public:
        font(asset_manager const &,
             char const *filename,
             std::size_t pixel_height,
             SDL_Color = {255, 255, 255, 255},
             std::source_location const & = std::source_location::current());

        TTF_Font *get() const noexcept { return pf.get(); }


        /// ### Measurements
        affine::extents2d const space, em;


        /// ### Text dimensions
        affine::extents2d measure(std::string_view text) const;


        /// ### Render text to a single line
        surface
                render(std::string_view text,
                       planet::ui::scale fit = planet::ui::scale::never) const {
            return render(text, colour, fit);
        }
        surface
                render(std::string_view text,
                       SDL_Color,
                       planet::ui::scale = planet::ui::scale::never) const;


        /// ### Wrap text if  it is too wide
        surface
                render(std::string_view text,
                       std::uint32_t const width,
                       planet::ui::scale fit = planet::ui::scale::never) const {
            return render(text, colour, width, fit);
        }
        surface
                render(std::string_view text,
                       SDL_Color,
                       std::uint32_t width,
                       planet::ui::scale = planet::ui::scale::never) const;
    };


}
