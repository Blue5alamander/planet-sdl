#include <planet/sdl/open_url.hpp>
#include <planet/sdl/sdl.hpp>


bool planet::sdl::open_url(char const *const url) noexcept {
    /// SDL3's `SDL_OpenURL` returns a `bool` that is `true` on success.
    return SDL_OpenURL(url);
}
