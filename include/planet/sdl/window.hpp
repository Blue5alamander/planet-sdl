#pragma once


#include <planet/sdl/renderer.hpp>

#include <SDL.h>

#include <cstdint>


namespace planet::sdl {


    class init;


    class window final {
        init &sdl;
        handle<SDL_Window, SDL_DestroyWindow> pw;
        affine::extent2d size;

      public:
        window(init &, const char *name, std::size_t width, std::size_t height);
        window(init &, const char *name, std::uint32_t flags);

        SDL_Window *get() const noexcept { return pw.get(); }

        sdl::renderer renderer;

        /// Current inner window size
        affine::extent2d const &extents() const { return size; }
        std::size_t zwidth() const noexcept { return size.zwidth(); }
        std::size_t zheight() const noexcept { return size.zheight(); }
        float width() const noexcept { return size.width(); }
        float height() const noexcept { return size.height(); }
    };


}
