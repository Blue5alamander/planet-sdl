#include <planet/sdl/event-loop.hpp>
#include <planet/sdl/init.hpp>
#include <planet/sdl/window.hpp>

#include <SDL.h>


using namespace std::literals;


/// ## `planet::sdl::event_loop`


felspar::coro::task<void> planet::sdl::event_loop::run() {
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    raw_mouse.push(
                            {planet::events::mouse::press::left,
                             planet::events::mouse::state::down,
                             planet::affine::point2d{
                                     float(event.motion.x),
                                     float(event.motion.y)}});
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    raw_mouse.push(
                            {planet::events::mouse::press::left,
                             planet::events::mouse::state::up,
                             planet::affine::point2d{
                                     float(event.motion.x),
                                     float(event.motion.y)}});
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                raw_mouse.push(
                        {planet::events::mouse::press::none,
                         planet::events::mouse::state::released,
                         planet::affine::point2d{
                                 float(event.motion.x), float(event.motion.y)}});
                break;
            case SDL_QUIT: quit.push({}); break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_CLOSE
                    and event.window.windowID == window_id) {
                    quit.push({});
                }
                break;
            default: break;
            }
        }
        co_await sdl.io.sleep(50ms);
    }
}
