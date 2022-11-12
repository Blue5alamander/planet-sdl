#pragma once


#include <planet/hexmap.hpp>
#include <planet/sdl/renderer.hpp>


namespace planet::sdl {


    /// Draw a hexagon at a specific location in the current colour
    void draw_hex(renderer &, panel &, hexmap::coordinates, float radius);


}
