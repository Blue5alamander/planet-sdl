#pragma once


#include <planet/asset_manager.hpp>
#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/ui/scale.hpp>

#include <SDL.h>
#undef main


namespace planet::sdl {


    class surface {
        handle<SDL_Surface, SDL_FreeSurface> ps;
        affine::extents2d size{{}, {}};

      public:
        using handle_type = decltype(ps);

        surface(handle_type h, ui::scale const f) : ps{std::move(h)}, fit{f} {
            if (ps.get()) { size = {float(ps->w), float(ps->h)}; }
        }

        /// Control how the surface is draw
        ui::scale fit;

        /// Create a surface by loading a BMP asset
        static surface load_bmp(
                asset_manager const &,
                char const *,
                felspar::source_location const & =
                        felspar::source_location::current());

        SDL_Surface *get() const noexcept { return ps.get(); }

        /// ### Return the texture extents
        /// The top left co-ordinates will always be (0, 0)
        affine::extents2d const &extents() const noexcept { return size; }
        float width() const noexcept { return size.width; }
        std::size_t uzwidth() const noexcept { return size.uzwidth(); }
        float height() const noexcept { return size.height; }
        std::size_t uzheight() const noexcept { return size.uzheight(); }
    };


}
