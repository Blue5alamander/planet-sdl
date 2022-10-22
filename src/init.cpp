#include <planet/sdl/init.hpp>
#include <SDL.h>


planet::sdl::init::init() { SDL_Init(SDL_INIT_VIDEO); }


planet::sdl::init::~init() { SDL_Quit(); }
