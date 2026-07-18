#pragma once


#include <planet/affine/extents2d.hpp>
#include <planet/sdl/sdl.hpp>


namespace planet::sdl {


    /// ## Display pixel density
    inline affine::extents2d pixel_density(SDL_Window *const window) noexcept
    /**
     * The number of drawable pixels per logical point for the display `window`
     * is on -- `> 1` on a HiDPI/Retina display and `1` otherwise. Mouse events
     * and display extents arrive in logical points whereas the swapchain works
     * in drawable pixels, so multiply the former by this to reach the latter.
     *
     * SDL reports a single density, so both axes carry the same value here.
     * Returning `affine::extents2d` (the aspect-ratio vocabulary used
     * elsewhere) leaves room for an anisotropic density without touching call
     * sites. Falls back to `{1, 1}` for a null window or when SDL cannot report
     * a density, so callers can multiply unconditionally.
     */
    {
        float const density = window ? SDL_GetWindowPixelDensity(window) : 0.0f;
        float const scale = density > 0.0f ? density : 1.0f;
        return {scale, scale};
    }

    inline affine::extents2d pixel_density(SDL_WindowID const window_id) noexcept
    /// Look the window up by id first. An unknown id yields `{1, 1}`.
    {
        return pixel_density(SDL_GetWindowFromID(window_id));
    }


}
