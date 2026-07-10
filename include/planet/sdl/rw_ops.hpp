#pragma once


#include <planet/sdl/handle.hpp>
#include <planet/sdl/sdl.hpp>
#include <planet/asset_manager.hpp>

#include <felspar/exceptions/runtime_error.hpp>


namespace planet::sdl {


    /**
     * SDL3 renamed `SDL_RWops` to `SDL_IOStream`. Using an alias lets the
     * member type and `get()` in `rw_ops_const_memory` be written once while
     * the creation/close calls branch on `PLANET_SDL3`.
     */
#if PLANET_SDL3
    using rw_stream = SDL_IOStream;
#else
    using rw_stream = SDL_RWops;
#endif


    /// ## SDL memory stream (`SDL_RWops` / `SDL_IOStream`) wrapper
    /**
     * Stores a vector of bytes that can be used by SDL's file loading through
     * its memory-backed IO abstraction — `SDL_RWops` on SDL2, `SDL_IOStream`
     * on SDL3.
     */
    class rw_ops_const_memory final {
        std::vector<std::byte> memory;
        rw_stream *prw;

      public:
        rw_ops_const_memory(
                std::vector<std::byte> m,
                std::source_location const &loc =
                        std::source_location::current())
        : memory{std::move(m)},
#if PLANET_SDL3
          prw{SDL_IOFromConstMem(memory.data(), memory.size())} {
#else
          prw{SDL_RWFromConstMem(memory.data(), memory.size())} {
#endif
                  if (not prw) {
                      throw felspar::stdexcept::runtime_error{
                              "SDL memory stream creation returned nullptr",
                              loc};
    }
}
rw_ops_const_memory(
        asset_manager const &am,
        char const *fn,
        std::source_location const &loc = std::source_location::current())
: rw_ops_const_memory{am.file_data(fn, loc), loc} {}
#if PLANET_SDL3
~rw_ops_const_memory() { SDL_CloseIO(prw); }
#else
        ~rw_ops_const_memory() { SDL_RWclose(prw); }
#endif

rw_stream *get() const noexcept { return prw; }
}
;
}
