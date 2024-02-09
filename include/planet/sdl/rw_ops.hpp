#pragma once


#include <planet/sdl/handle.hpp>
#include <planet/asset_manager.hpp>

#include <felspar/exceptions.hpp>

#include <SDL.h>
#undef main


namespace planet::sdl {


    class rw_ops_const_memory final {
        std::vector<std::byte> memory;
        SDL_RWops *prw;

      public:
        rw_ops_const_memory(
                std::vector<std::byte> m,
                felspar::source_location const &loc =
                        felspar::source_location::current())
        : memory{std::move(m)},
          prw{SDL_RWFromConstMem(memory.data(), memory.size())} {
            if (not prw) {
                throw felspar::stdexcept::runtime_error{
                        "SDL_RWFromConstMem returned nullptr", loc};
            }
        }
        rw_ops_const_memory(
                asset_manager const &am,
                char const *fn,
                felspar::source_location const &loc =
                        felspar::source_location::current())
        : rw_ops_const_memory{am.file_data(fn, loc)} {}
        ~rw_ops_const_memory() { SDL_RWclose(prw); }

        SDL_RWops *get() const noexcept { return prw; }
    };


}
