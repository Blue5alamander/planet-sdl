#include <planet/sdl/open_url.hpp>
#include <planet/sdl/sdl.hpp>


bool planet::sdl::open_url(char const *const url) noexcept {
    /**
     * SDL2's `SDL_OpenURL` returns `0` on success and non-zero on failure. SDL3
     * flips this to a `bool` that is `true` on success, so only this comparison
     * needs to change when the `PLANET_SDL3` toggle is flipped on in a later
     * phase — every caller is already written against this success-`bool`
     * interface and needs no edit.
     */
    return SDL_OpenURL(url) == 0;
}
