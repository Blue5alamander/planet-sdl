#pragma once


#include <planet/sdl/panel.hpp>
#include <planet/sdl/handle.hpp>

#include <felspar/coro/stream.hpp>
#include <felspar/exceptions.hpp>


#include <SDL.h>

#include <cstdint>
#include <span>


namespace planet::sdl {


    class window;


    /// If a drawing API fails then throw an exception
    inline int drawing_worked(
            int const e,
            felspar::source_location const &loc =
                    felspar::source_location::current()) {
        if (e < 0) [[unlikely]] {
            throw felspar::stdexcept::runtime_error{
                    std::string{"Drawing API failed "} + SDL_GetError()};
        } else {
            return e;
        }
    }


    class renderer final {
        friend class window;
        window &win;
        handle<SDL_Renderer, SDL_DestroyRenderer> pr;

        /// Only creatable by the window
        renderer(window &);

      public:
        SDL_Renderer *get() const noexcept { return pr.get(); }

        /// A representation of the screen the renderer is drawing to. Using
        /// this allows for global management of the coordinate system the
        /// application uses
        panel screen = {*this};


        /// ## Render function

        /**
         * Describe the last frame that has been rendered. Typically this is
         * done with an instance of the `render_loop` using something like:
         * ```cpp
         * co_yield renderer.present();
         * ```
         */
        struct frame {
            std::size_t number = {};
        };


        /// ## Frame presentation

        /// Clear draw commands ready for next frame
        void clear();
        /// Send the current draw commands to the screen
        [[nodiscard]] frame present();


        /// ## Graphics APIs
        /**
         * All of these APIs use the SDL coordinate space, with no adjustment
         * form `screen` member coordinate space.
         */

        /// Graphics APIs in pixel coordinate space
        void colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) const;
        void colour(SDL_Color const &) const;
        void
                line(std::size_t x1,
                     std::size_t y1,
                     std::size_t x2,
                     std::size_t y2) const;
        void lines(std::span<SDL_Point>) const;
        void copy(texture const &, std::size_t x, std::size_t y);
        void
                copy(texture const &,
                     std::size_t x,
                     std::size_t y,
                     std::size_t w,
                     std::size_t h);

      private:
        frame current_frame = {};
    };


}
