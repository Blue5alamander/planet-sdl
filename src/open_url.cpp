#include <planet/sdl/open_url.hpp>
#include <planet/sdl/sdl.hpp>


bool planet::sdl::open_url(char const *const url) noexcept {
    /**
     * Every caller is written against this success-`bool` interface, so only
     * the SDL return convention differs between the two branches.
     */
#if PLANET_SDL3
    /// SDL3's `SDL_OpenURL` returns a `bool` that is `true` on success.
    return SDL_OpenURL(url);
#else
    /// SDL2's `SDL_OpenURL` returns `0` on success and non-zero on failure.
    return SDL_OpenURL(url) == 0;
#endif
}
