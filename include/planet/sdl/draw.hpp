#pragma once


#include <planet/map/hex.hpp>
#include <planet/sdl/renderer.hpp>


namespace planet::sdl {


    /// Draw a hexagon at a specific location in the current colour
    void draw_hex(renderer &, ui::panel &, map::hex::coordinates, float radius);


}
