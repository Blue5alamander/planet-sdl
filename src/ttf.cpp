#include <planet/sdl/ttf.hpp>


/**
 * `planet::sdl::ttf`
 */


planet::sdl::ttf::ttf(init &) { TTF_Init(); }


planet::sdl::ttf::~ttf() { TTF_Quit(); }


/**
 * `planet::sdl::font`
 */


planet::sdl::font::font(char const *filename, std::size_t pixels)
: pf{TTF_OpenFont(filename, pixels)} {}
