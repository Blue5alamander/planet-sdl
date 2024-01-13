#include <planet/log.hpp>
#include <planet/sdl/event-loop.hpp>
#include <planet/sdl/init.hpp>
#include <planet/sdl/window.hpp>

#include <SDL.h>


using namespace std::literals;


/// ## `planet::sdl::event_loop`


felspar::coro::task<void>
        planet::sdl::event_loop::forward_to_baseplate(ui::baseplate &bp) {
    felspar::coro::starter<> forwarders;
    forwarders.post(
            events.key, &queue::pmc<planet::events::key>::forward<>,
            std::ref(bp.events.key));
    forwarders.post(
            events.mouse, &queue::pmc<planet::events::mouse>::forward<>,
            std::ref(bp.events.mouse));
    forwarders.post(
            events.scroll, &queue::pmc<planet::events::scroll>::forward<>,
            std::ref(bp.events.scroll));
    co_await forwarders.wait_for_all();
}


felspar::coro::task<void> planet::sdl::event_loop::run() {
    planet::log::info("planet::sdl::event_loop::run");
    affine::point2d last_mouse_pos{{}, {}};
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_KEYDOWN:
                events.key.push(
                        {static_cast<events::scancode>(
                                 event.key.keysym.scancode),
                         events::action::down});
                break;

            case SDL_MOUSEBUTTONDOWN:
                last_mouse_pos = {float(event.motion.x), float(event.motion.y)};
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    events.mouse.push(
                            {events::button::left, events::action::down,
                             last_mouse_pos, event.button.clicks});
                    break;
                case SDL_BUTTON_RIGHT:
                    events.mouse.push(
                            {events::button::right, events::action::down,
                             last_mouse_pos, event.button.clicks});
                    break;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                last_mouse_pos = {float(event.motion.x), float(event.motion.y)};
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    events.mouse.push(
                            {events::button::left, events::action::up,
                             last_mouse_pos, event.button.clicks});
                    break;
                case SDL_BUTTON_RIGHT:
                    events.mouse.push(
                            {events::button::right, events::action::up,
                             last_mouse_pos, event.button.clicks});
                    break;
                }
                break;

            case SDL_MOUSEMOTION:
                last_mouse_pos = {float(event.motion.x), float(event.motion.y)};
                {
                    events::action a = events::action::released;
                    events::button b = events::button::none;
                    if (event.motion.state bitand SDL_BUTTON_LMASK) {
                        a = events::action::held;
                        b = events::button::left;
                    } else if (event.motion.state bitand SDL_BUTTON_RMASK) {
                        a = events::action::held;
                        b = events::button::right;
                    }
                    events.mouse.push(
                            {b, a, last_mouse_pos, event.button.clicks});
                }
                break;

            case SDL_MOUSEWHEEL:
                events.scroll.push(
                        {event.wheel.preciseX, event.wheel.preciseY,
                         last_mouse_pos});
                break;

            case SDL_QUIT: events.quit.push({}); break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_CLOSE
                    and event.window.windowID == window_id) {
                    events.quit.push({});
                }
                break;

            default: break;
            }
        }
        co_await sdl.io.sleep(30ms);
    }
}
