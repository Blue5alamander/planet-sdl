#include <planet/sdl/rw_ops.hpp>
#include <planet/sdl/surface.hpp>


planet::sdl::surface planet::sdl::surface::load_bmp(
        asset_manager const &am,
        char const *fn,
        std::source_location const &loc) {
    rw_ops_const_memory data{am.file_data(fn, loc)};
    return {SDL_LoadBMP_RW(data.get(), false), ui::scale::lock_aspect};
}
