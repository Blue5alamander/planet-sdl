#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>

#include <SDL.h>

#include <cstdint>
#include <span>


namespace planet::sdl {


    class renderer;
    class window;


    /// Drawing onto a renderer for a frame
    class drawframe final {
        renderer &rend;
        std::size_t w, h;

      public:
        drawframe(renderer &, std::uint8_t r, std::uint8_t g, std::uint8_t b);
        ~drawframe();

        transform viewport = {};

        std::size_t width() const noexcept { return w; }
        std::size_t height() const noexcept { return h; }

        void colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) const;
        void
                line(std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2) const;
        void lines(std::span<SDL_Point>) const;
    };


    class renderer final {
        friend drawframe;
        window &win;
        handle<SDL_Renderer, SDL_DestroyRenderer> pr;

      public:
        renderer(window &);

        SDL_Renderer *get() const noexcept { return pr.get(); }

        drawframe operator()(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
            return {*this, r, g, b};
        }
    };


}
