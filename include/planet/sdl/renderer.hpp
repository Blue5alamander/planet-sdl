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


    /// Co-ordinate transform hierarchy for world etc. co-ordinate systems
    class drawframe final {
        renderer &rend;

      public:
        drawframe(renderer &);

        affine::transform viewport = {};

        /// Draw a line between two points in world co-ordinate space
        void line(affine::point2d, affine::point2d) const;
    };


    class renderer final {
        friend drawframe;
        window &win;
        handle<SDL_Renderer, SDL_DestroyRenderer> pr;

      public:
        renderer(window &);

        SDL_Renderer *get() const noexcept { return pr.get(); }

        /// Graphics APIs in pixel coordinate space
        void colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) const;
        void
                line(std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2) const;
        void lines(std::span<SDL_Point>) const;
        void copy(texture const &, std::size_t x, std::size_t y);

        /// Clear draw commands ready for next frame
        void clear() const;
        /// Send the current draw commands to the screen
        void present() const;
    };


}
