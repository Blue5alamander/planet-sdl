#pragma once


#include <planet/asset_manager.hpp>
#include <planet/sdl/handle.hpp>

#include <SDL.h>


namespace planet::sdl {


    class surface {
      public:
        using handle_type = handle<SDL_Surface, SDL_FreeSurface>;

        surface(handle_type h) : ps{std::move(h)} {}

        SDL_Surface *get() const noexcept { return ps.get(); }

        /// Create a surface by loading a BMP asset
        static surface load_bmp(
                asset_manager const &,
                char const *,
                felspar::source_location const & =
                        felspar::source_location::current());

      private:
        handle_type ps;
    };


}
