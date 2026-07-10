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

#if PLANET_SDL3
            case SDL_EVENT_KEY_DOWN: {
#else
            case SDL_KEYDOWN: {
#endif
                auto const scancode =
#if PLANET_SDL3
                        event.key.scancode;
#else
                        event.key.keysym.scancode;
#endif
                planet::log::debug(
                        "SDL_KEYDOWN", static_cast<int>(scancode));
                events.key.push(
                        {static_cast<events::scancode>(scancode),
                         events::action::down});
                break;
            }
#if PLANET_SDL3
            case SDL_EVENT_KEY_UP: {
#else
            case SDL_KEYUP: {
#endif
                auto const scancode =
#if PLANET_SDL3
                        event.key.scancode;
#else
                        event.key.keysym.scancode;
#endif
                planet::log::debug(
                        "SDL_KEYUP", static_cast<int>(scancode));
                events.key.push(
                        {static_cast<events::scancode>(scancode),
                         events::action::up});
                break;
            }

#if PLANET_SDL3
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
#else
            case SDL_MOUSEBUTTONDOWN:
#endif
                last_mouse_pos =
#if PLANET_SDL3
                        {event.motion.x, event.motion.y};
#else
                        {float(event.motion.x), float(event.motion.y)};
#endif
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

#if PLANET_SDL3
            case SDL_EVENT_MOUSE_BUTTON_UP:
#else
            case SDL_MOUSEBUTTONUP:
#endif
                last_mouse_pos =
#if PLANET_SDL3
                        {event.motion.x, event.motion.y};
#else
                        {float(event.motion.x), float(event.motion.y)};
#endif
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

#if PLANET_SDL3
            case SDL_EVENT_MOUSE_MOTION:
#else
            case SDL_MOUSEMOTION:
#endif
                last_mouse_pos =
#if PLANET_SDL3
                        {event.motion.x, event.motion.y};
#else
                        {float(event.motion.x), float(event.motion.y)};
#endif
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

#if PLANET_SDL3
            case SDL_EVENT_MOUSE_WHEEL:
#else
            case SDL_MOUSEWHEEL:
#endif
                events.scroll.push(
#if PLANET_SDL3
                        {event.wheel.x, event.wheel.y, last_mouse_pos});
#else
                        {event.wheel.preciseX, event.wheel.preciseY,
                         last_mouse_pos});
#endif
                break;

#if PLANET_SDL3
            case SDL_EVENT_QUIT:
#else
            case SDL_QUIT:
#endif
                events.quit.push({});
                break;

#if not PLANET_SDL3
                /**
                 * SDL3 removed the `SDL_WINDOWEVENT` event type entirely,
                 * splitting it into top-level `SDL_EVENT_WINDOW_*` events
                 * (close-requested, focus lost/gained). Those are wired up in
                 * chunk 6.2; until then this block is SDL2-only so the SDL3
                 * build still compiles.
                 */
            case SDL_WINDOWEVENT:
                if (event.window.windowID != window_id) {
                    planet::log::warning(
                            "Got SDL_WINDOWEVENT for a window ID that isn't mine. Mine",
                            window_id, "event", event.window.windowID);
                } else if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    events.quit.push({});
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    planet::log::info("Window lost focus");
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    planet::log::info("Window gained focus");
                }
                break;
#endif

            default: break;
            }
        }
        co_await sdl.io.sleep(30ms);
    }
}
