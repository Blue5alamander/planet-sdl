#pragma once


#include <planet/sdl/renderer.hpp>
#include <planet/events/mouse.hpp>
#include <planet/ui/baseplate.hpp>

#include <felspar/coro/start.hpp>

#include <SDL.h>

#include <cstdint>


namespace planet::sdl {


    class init;


    class window final {
        handle<SDL_Window, SDL_DestroyWindow> pw;
        affine::extents2d size;
        felspar::coro::starter<> processes;

      public:
        window(init &,
               const char *name,
               int posx,
               int posy,
               int width,
               int height,
               std::uint32_t flags = {});
        window(init &,
               const char *name,
               std::size_t width,
               std::size_t height,
               std::uint32_t flags = {});
        window(init &,
               const char *name,
               std::uint32_t flags = SDL_WINDOW_FULLSCREEN_DESKTOP);

        SDL_Window *get() const noexcept { return pw.get(); }

        sdl::renderer renderer;
        ui::baseplate<sdl::renderer> baseplate;

        /// Current inner window size
        affine::extents2d const &extents() const noexcept { return size; }
        affine::rectangle2d rectangle() const noexcept {
            return {{0, 0}, size};
        }
        std::size_t zwidth() const noexcept { return size.zwidth(); }
        std::size_t zheight() const noexcept { return size.zheight(); }
        float width() const noexcept { return size.width; }
        float height() const noexcept { return size.height; }
    };


}
