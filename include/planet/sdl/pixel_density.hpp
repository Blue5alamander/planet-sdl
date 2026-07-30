#pragma once


#include <planet/affine/rectangle2d.hpp>
#include <planet/sdl/sdl.hpp>

#include <cmath>


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


    /// ## Back from drawable pixels to window points
    /**
     * The way back out of the UI's coordinates. The event loop scales pointer
     * positions up by the density on the way in, so everything laid out is in
     * drawable pixels -- but SDL's own window APIs are all in logical window
     * points, and anything handed back to one of them has to be divided down
     * again.
     *
     * Take the density from `pixel_density()`, which never reports zero even
     * for a window SDL knows nothing about.
     */

    /// ### A UI length in window points
    inline int window_length(float const pixels, float const density) noexcept {
        return static_cast<int>(pixels / density);
    }

    /// ### A UI rectangle in window points
    inline SDL_Rect window_rectangle(
            affine::rectangle2d const &r,
            affine::extents2d const density) noexcept
    /**
     * Rounded outwards, so the rectangle SDL is told about always covers the
     * whole of the one that was laid out rather than clipping a fraction of a
     * point off an edge.
     */
    {
        float const left = std::floor(r.left() / density.width);
        float const top = std::floor(r.top() / density.height);
        float const right = std::ceil(r.right() / density.width);
        float const bottom = std::ceil(r.bottom() / density.height);
        return {.x = static_cast<int>(left),
                .y = static_cast<int>(top),
                .w = static_cast<int>(right - left),
                .h = static_cast<int>(bottom - top)};
    }


}
