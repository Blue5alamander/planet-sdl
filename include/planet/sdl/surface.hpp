#pragma once


#include <planet/asset_manager.hpp>
#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>

#include <SDL.h>


namespace planet::sdl {


    class surface {
        handle<SDL_Surface, SDL_FreeSurface> ps;
        affine::extents2d size;

      public:
        using handle_type = decltype(ps);

        surface(handle_type h)
        : ps{std::move(h)}, size{float(ps->w), float(ps->h)} {}

        /// Create a surface by loading a BMP asset
        static surface load_bmp(
                asset_manager const &,
                char const *,
                felspar::source_location const & =
                        felspar::source_location::current());

        SDL_Surface *get() const noexcept { return ps.get(); }

        /// Return the texture extents. The top left co-ordinates will always be
        /// 0, 0
        affine::extents2d const &extents() const noexcept { return size; }
        float width() const { return size.width; }
        std::size_t zwidth() const { return size.zwidth(); }
        float height() const { return size.height; }
        std::size_t zheight() const { return size.zheight(); }
    };


}
