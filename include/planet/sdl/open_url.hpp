#pragma once


namespace planet::sdl {


    /// ## Open a URL in the user's default handler
    /**
     * Wraps `SDL_OpenURL` so that callers do not have to deal with the return
     * convention changing between SDL2 (an `int` that is `0` on success) and
     * SDL3 (a `bool` that is `true` on success).
     *
     * Returns `true` when the URL was handed off to the platform successfully.
     */
    bool open_url(char const *url) noexcept;


}
