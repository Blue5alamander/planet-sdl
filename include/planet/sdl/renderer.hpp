#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>

#include <SDL.h>

#include <cstdint>
#include <span>


namespace planet::sdl {


    class renderer;
    class texture;
    class window;


    /// Drawing onto a renderer for a frame
    class drawframe final {
        renderer &rend;
        std::size_t w, h;

      public:
        drawframe(renderer &, std::uint8_t r, std::uint8_t g, std::uint8_t b);
        ~drawframe();

        affine::transform viewport = {};

        std::size_t width() const noexcept { return w; }
        std::size_t height() const noexcept { return h; }

        void colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) const;
        void
                line(std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2) const;
        void lines(std::span<SDL_Point>) const;
        void copy(texture const &, std::size_t x, std::size_t y);

        /// Draw a line between two points in world co-ordinate space
        void line(affine::point2d const cp1, affine::point2d const cp2) const {
            auto p1 = viewport.into(cp1);
            auto p2 = viewport.into(cp2);
            line(p1.x(), p1.y(), p2.x(), p2.y());
        }
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
