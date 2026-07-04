#pragma once


#include <planet/sdl/forward.hpp>
#include <planet/sdl/renderer.hpp>
#include <planet/events/mouse.hpp>
#include <planet/ui/baseplate.hpp>
#include <planet/ui/constrained.hpp>

#include <felspar/coro/starter.hpp>

#include <SDL.h>
#undef main

#include <cstdint>


namespace planet::sdl {


    class window final {
        handle<SDL_Window, SDL_DestroyWindow> pw;
        affine::extents2d desktop_size, window_size;
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
        ui::baseplate baseplate;

        /// Full screen resolution of the display the window is on
        affine::extents2d const &display_extents() const noexcept {
            return desktop_size;
        }

        /// Current inner window size
        affine::extents2d const &extents() const noexcept {
            return window_size;
        }
        affine::rectangle2d rectangle() const noexcept {
            return {{0, 0}, window_size};
        }
        std::size_t uzwidth() const noexcept { return window_size.uzwidth(); }
        std::size_t uzheight() const noexcept { return window_size.uzheight(); }
        float width() const noexcept { return window_size.width; }
        float height() const noexcept { return window_size.height; }

        using constrained_type = ui::constrained2d<float>;
        constrained_type constraints() const {
            return {{window_size.width, 0, window_size.width},
                    {window_size.height, 0, window_size.height}};
        }
    };


}
