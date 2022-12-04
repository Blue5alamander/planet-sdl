#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/ui/scale.hpp>

#include <SDL.h>


namespace planet::sdl {


    class renderer;
    class surface;


    class texture {
        handle<SDL_Texture, SDL_DestroyTexture> pt;
        affine::extents2d size;

      public:
        texture(renderer &, surface const &);

        SDL_Texture *get() const noexcept { return pt.get(); }

        /// Control how the texture is draw
        ui::scale fit;

        /// Return the texture extents. The top left co-ordinates will always be
        /// 0, 0
        affine::extents2d const &extents() const noexcept { return size; }
        affine::extents2d extents(affine::extents2d) const noexcept;
        float width() const { return size.width; }
        std::size_t zwidth() const { return size.zwidth(); }
        float height() const { return size.height; }
        std::size_t zheight() const { return size.zheight(); }

        /// Draw the texture at the given screen co-ordinates
        void draw_within(renderer &, affine::rectangle const &) const;
    };


}
