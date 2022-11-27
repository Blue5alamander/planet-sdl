#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>

#include <SDL.h>


namespace planet::sdl {


    class renderer;
    class surface;


    class texture {
        handle<SDL_Texture, SDL_DestroyTexture> pt;
        affine::extent2d size;

      public:
        texture(renderer &, surface const &);

        SDL_Texture *get() const noexcept { return pt.get(); }

        /// Return the texture extents. The top left co-ordinates will always be
        /// 0, 0
        affine::extent2d const &extents() const noexcept { return size; }
        float width() const { return size.width(); }
        std::size_t zwidth() const { return size.zwidth(); }
        float height() const { return size.height(); }
        std::size_t zheight() const { return size.height(); }

        /// Draw the texture at the given screen co-ordinates
        void draw_within(renderer &, affine::extent2d const &) const;
    };


}
