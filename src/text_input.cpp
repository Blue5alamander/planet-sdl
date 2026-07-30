#include <planet/log.hpp>
#include <planet/sdl/pixel_density.hpp>
#include <planet/sdl/text_input.hpp>


void planet::sdl::set_text_input_area(
        SDL_Window *const window,
        affine::rectangle2d const &area,
        float const cursor) noexcept {
    auto const density = pixel_density(window);
    SDL_Rect const rect = window_rectangle(area, density);
    if (not SDL_SetTextInputArea(
                window, &rect, window_length(cursor, density.width))) {
        log::warning("SDL_SetTextInputArea failed", SDL_GetError());
    }
}


void planet::sdl::start_text_input(SDL_Window *const window) noexcept {
    if (not SDL_StartTextInput(window)) {
        log::warning("SDL_StartTextInput failed", SDL_GetError());
    }
}


void planet::sdl::stop_text_input(SDL_Window *const window) noexcept {
    if (not SDL_StopTextInput(window)) {
        log::warning("SDL_StopTextInput failed", SDL_GetError());
    }
}
