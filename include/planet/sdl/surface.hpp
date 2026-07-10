#pragma once


#include <planet/asset_manager.hpp>
#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/sdl/sdl.hpp>
#include <planet/ui/scale.hpp>


namespace planet::sdl {


    /// ## Surface blend modes
    /**
     * Mirrors the subset of SDL's `SDL_BlendMode` values the wrapper needs so
     * that callers do not have to touch the raw SDL enum. The SDL enum values
     * are unchanged between SDL2 and SDL3.
     */
    enum class blend_mode { none, blend, add, mod };


    /// ## SDL Surface
    class surface {
        handle<SDL_Surface, SDL_FreeSurface> ps;
        affine::extents2d size{{}, {}};

      public:
        using handle_type = decltype(ps);

        surface(handle_type h, ui::scale const f) : ps{std::move(h)}, fit{f} {
            if (ps.get()) { size = {float(ps->w), float(ps->h)}; }
        }

        /// ### Control how the surface is draw
        ui::scale fit;

        /// ### Create a surface by loading a BMP asset
        static surface load_bmp(
                asset_manager const &,
                char const *,
                std::source_location const & = std::source_location::current());

        /// ### Create a blank ARGB8888 surface of the requested pixel size
        static surface create_argb8888(
                std::size_t width,
                std::size_t height,
                ui::scale const fit = ui::scale::lock_aspect) noexcept;

        SDL_Surface *get() const noexcept { return ps.get(); }

        /// ### Return the texture extents
        /// The top left co-ordinates will always be (0, 0)
        affine::extents2d const &extents() const noexcept { return size; }
        float width() const noexcept { return size.width; }
        std::size_t uzwidth() const noexcept { return size.uzwidth(); }
        float height() const noexcept { return size.height; }
        std::size_t uzheight() const noexcept { return size.uzheight(); }

        /// ### Blit this surface onto the top-left corner `(x, y)` of another
        void blit_onto(surface const &destination, std::size_t x, std::size_t y)
                const noexcept;

        /// ### Set the blend mode used when this surface is blitted
        void set_blend_mode(blend_mode const mode) const noexcept;
    };


}
