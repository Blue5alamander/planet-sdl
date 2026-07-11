#pragma once


#include <planet/sdl/handle.hpp>
#include <planet/sdl/sdl.hpp>
#include <planet/asset_manager.hpp>

#include <felspar/exceptions/runtime_error.hpp>


namespace planet::sdl {


    /// ## SDL memory stream (`SDL_IOStream`) wrapper
    /**
     * Stores a vector of bytes that can be used by SDL's file loading through
     * its memory-backed IO abstraction (`SDL_IOStream`).
     */
    class rw_ops_const_memory final {
        std::vector<std::byte> memory;
        SDL_IOStream *prw;

      public:
        rw_ops_const_memory(
                std::vector<std::byte> m,
                std::source_location const &loc =
                        std::source_location::current())
        : memory{std::move(m)},
          prw{SDL_IOFromConstMem(memory.data(), memory.size())} {
            if (not prw) {
                throw felspar::stdexcept::runtime_error{
                        "SDL memory stream creation returned nullptr", loc};
            }
        }
        rw_ops_const_memory(
                asset_manager const &am,
                char const *fn,
                std::source_location const &loc =
                        std::source_location::current())
        : rw_ops_const_memory{am.file_data(fn, loc), loc} {}
        ~rw_ops_const_memory() { SDL_CloseIO(prw); }

        SDL_IOStream *get() const noexcept { return prw; }
    };


}
