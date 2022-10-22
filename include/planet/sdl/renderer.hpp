#pragma once


#include <SDL.h>

#include <cstdint>


namespace planet::sdl {


    class renderer;
    class window;


    /// Drawing onto a renderer for a frame
    class drawframe final {
        renderer &rend;

      public:
        drawframe(renderer &, std::uint8_t r, std::uint8_t g, std::uint8_t b);
        ~drawframe();
    };


    class renderer final {
        window &win;
        SDL_Renderer *pr = nullptr;

      public:
        renderer(window &);
        ~renderer();

        operator SDL_Renderer *() const noexcept { return pr; }

        drawframe operator()(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
            return {*this, r, g, b};
        }
    };


}
