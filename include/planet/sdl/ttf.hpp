#pragma once


#include <planet/affine/extents2d.hpp>
#include <planet/asset_manager.hpp>
#include <planet/sdl/forward.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/sdl/rw_ops.hpp>
#include <planet/sdl/surface.hpp>

#include <SDL3_ttf/SDL_ttf.h>

#include <optional>


namespace planet::sdl {


    /// ## Initialise TTF
    /// Create one of these for access to the TTF rendering parts of SDL3
    class ttf {
      public:
        ttf(init &);
        ~ttf();
    };


    /// ## TTF Font
    class font {
        rw_ops_const_memory font_data;
        /**
         * A font consulted for glyphs the primary font does not contain, kept
         * only when a fallback filename was supplied. Both it and its
         * `TTF_Font` are declared before `pf` so that on destruction the
         * primary font is closed first: SDL holds the fallback inside the
         * primary, so the fallback must outlive it.
         */
        std::optional<rw_ops_const_memory> fallback_data;
        handle<TTF_Font, TTF_CloseFont> fallback_pf, pf;
        SDL_Color colour;

      public:
        /// ### Load a font, optionally with a fallback
        /**
         * `fallback_filename`, when given, names a font that supplies glyphs
         * the primary font lacks — the musical accidentals in note names, for
         * instance, which the UI fonts do not carry. It is opened at the same
         * pixel height so the substituted glyphs match the surrounding text.
         */
        font(asset_manager const &,
             char const *filename,
             std::size_t pixel_height,
             char const *fallback_filename = nullptr,
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
