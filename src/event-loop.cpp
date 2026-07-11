#include <planet/log.hpp>
#include <planet/sdl/event-loop.hpp>
#include <planet/sdl/init.hpp>
#include <planet/sdl/sdl.hpp>


using namespace std::literals;


/// ## `planet::sdl::event_loop`


felspar::coro::task<void>
        planet::sdl::event_loop::forward_to_baseplate(ui::baseplate &bp) {
    felspar::coro::starter<> forwarders;
    forwarders.post(events.key.forward(bp.events.key));
    forwarders.post(events.mouse.forward(bp.events.mouse));
    forwarders.post(events.scroll.forward(bp.events.scroll));
    co_await forwarders.wait_for_all();
}


felspar::coro::task<void> planet::sdl::event_loop::run() {
    planet::log::info("planet::sdl::event_loop::run");
    affine::point2d last_mouse_pos{{}, {}};
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_EVENT_KEY_DOWN: {
                auto const scancode = event.key.scancode;
                planet::log::debug(
                        "SDL_EVENT_KEY_DOWN", static_cast<int>(scancode));
                events.key.push(
                        {static_cast<events::scancode>(scancode),
                         events::action::down});
                break;
            }
            case SDL_EVENT_KEY_UP: {
                auto const scancode = event.key.scancode;
                planet::log::debug(
                        "SDL_EVENT_KEY_UP", static_cast<int>(scancode));
                events.key.push(
                        {static_cast<events::scancode>(scancode),
                         events::action::up});
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                last_mouse_pos = {event.motion.x, event.motion.y};
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

            case SDL_EVENT_MOUSE_BUTTON_UP:
                last_mouse_pos = {event.motion.x, event.motion.y};
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

            case SDL_EVENT_MOUSE_MOTION:
                last_mouse_pos = {event.motion.x, event.motion.y};
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

            case SDL_EVENT_MOUSE_WHEEL:
                events.scroll.push(
                        {event.wheel.x, event.wheel.y, last_mouse_pos});
                break;

            case SDL_EVENT_QUIT: events.quit.push({}); break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID != window_id) {
                    planet::log::warning(
                            "Got SDL_WINDOWEVENT for a window ID that isn't mine. Mine",
                            window_id, "event", event.window.windowID);
                } else {
                    events.quit.push({});
                }
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST: [[fallthrough]];
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                if (event.window.windowID != window_id) {
                    planet::log::warning(
                            "Got SDL_WINDOWEVENT for a window ID that isn't mine. Mine",
                            window_id, "event", event.window.windowID);
                } else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
                    planet::log::info("Window lost focus");
                } else if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
                    planet::log::info("Window gained focus");
                }
                break;

            default: break;
            }
        }
        co_await sdl.io.sleep(30ms);
    }
}
