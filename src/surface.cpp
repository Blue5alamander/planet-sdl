#include <planet/sdl/rw_ops.hpp>
#include <planet/sdl/surface.hpp>


planet::sdl::surface planet::sdl::surface::load_bmp(
        asset_manager const &am,
        char const *fn,
        std::source_location const &loc) {
    rw_ops_const_memory data{am.file_data(fn, loc)};
    return {SDL_LoadBMP_RW(data.get(), false), ui::scale::lock_aspect};
}


planet::sdl::surface planet::sdl::surface::create_argb8888(
        std::size_t const width,
        std::size_t const height,
        ui::scale const fit) noexcept {
    return {SDL_CreateRGBSurfaceWithFormat(
                    0, static_cast<int>(width), static_cast<int>(height), 32,
                    SDL_PIXELFORMAT_ARGB8888),
            fit};
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
