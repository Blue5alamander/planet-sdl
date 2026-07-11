#include <planet/sdl/rw_ops.hpp>
#include <planet/sdl/surface.hpp>


planet::sdl::surface planet::sdl::surface::load_bmp(
        asset_manager const &am,
        char const *fn,
        std::source_location const &loc) {
    rw_ops_const_memory data{am.file_data(fn, loc)};
#if PLANET_SDL3
    return {SDL_LoadBMP_IO(data.get(), false), ui::scale::lock_aspect};
#else
    return {SDL_LoadBMP_RW(data.get(), false), ui::scale::lock_aspect};
#endif
}


planet::sdl::surface planet::sdl::surface::create_argb8888(
        std::size_t const width,
        std::size_t const height,
        ui::scale const fit) noexcept {
    /**
     * SDL2's `SDL_CreateRGBSurfaceWithFormat` takes flags, depth and the pixel
     * format; SDL3's `SDL_CreateSurface` drops the now-unused flags and depth
     * (it infers the depth from the format).
     */
#if PLANET_SDL3
    return {SDL_CreateSurface(
                    static_cast<int>(width), static_cast<int>(height),
                    SDL_PIXELFORMAT_ARGB8888),
            fit};
#else
    return {SDL_CreateRGBSurfaceWithFormat(
                    0, static_cast<int>(width), static_cast<int>(height), 32,
                    SDL_PIXELFORMAT_ARGB8888),
            fit};
#endif
}


void planet::sdl::surface::blit_onto(
        surface const &destination,
        std::size_t const x,
        std::size_t const y) const noexcept {
    SDL_Rect dst{
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(uzwidth()), static_cast<int>(uzheight())};
    SDL_BlitSurface(get(), nullptr, destination.get(), &dst);
}


void planet::sdl::surface::set_blend_mode(blend_mode const mode) const noexcept {
    SDL_BlendMode sdl_mode = SDL_BLENDMODE_NONE;
    switch (mode) {
    case blend_mode::blend: sdl_mode = SDL_BLENDMODE_BLEND; break;
    case blend_mode::add: sdl_mode = SDL_BLENDMODE_ADD; break;
    case blend_mode::mod: sdl_mode = SDL_BLENDMODE_MOD; break;
    case blend_mode::none: sdl_mode = SDL_BLENDMODE_NONE; break;
    }
    SDL_SetSurfaceBlendMode(get(), sdl_mode);
}


std::uint32_t planet::sdl::surface::map_rgba(
        std::uint8_t const r,
        std::uint8_t const g,
        std::uint8_t const b,
        std::uint8_t const a) const noexcept {
    /**
     * SDL3 flattened `SDL_Surface::format` from a pointer-to-details struct to
     * a plain enum, so `SDL_MapRGBA` can no longer be handed it directly;
     * `SDL_MapSurfaceRGBA` maps against the surface's own format instead. SDL2
     * keeps the `SDL_PixelFormat *` in `format`.
     */
#if PLANET_SDL3
    return SDL_MapSurfaceRGBA(get(), r, g, b, a);
#else
    return SDL_MapRGBA(get()->format, r, g, b, a);
#endif
}


void planet::sdl::surface::fill(std::uint32_t const colour) const noexcept {
    /// SDL3 renamed `SDL_FillRect` to `SDL_FillSurfaceRect`.
#if PLANET_SDL3
    SDL_FillSurfaceRect(get(), nullptr, colour);
#else
    SDL_FillRect(get(), nullptr, colour);
#endif
}


void planet::sdl::surface::fill_rect(
        std::size_t const x,
        std::size_t const y,
        std::size_t const width,
        std::size_t const height,
        std::uint32_t const colour) const noexcept {
    SDL_Rect const rect{
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(width),
            static_cast<int>(height)};
#if PLANET_SDL3
    SDL_FillSurfaceRect(get(), &rect, colour);
#else
    SDL_FillRect(get(), &rect, colour);
#endif
}
