#include <planet/sdl/init.hpp>
#include <SDL.h>


planet::sdl::init::init(felspar::io::warden &w) : io{w} {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}


planet::sdl::init::~init() { SDL_Quit(); }
