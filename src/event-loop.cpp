#include <planet/log.hpp>
#include <planet/sdl/event-loop.hpp>
#include <planet/sdl/init.hpp>
#include <planet/sdl/pixel_density.hpp>
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
        /**
         * Mouse events arrive in logical window points, but the swapchain and
         * UI work in drawable pixels (the window is created
         * `SDL_WINDOW_HIGH_PIXEL_DENSITY`), so scale pointer positions by the
         * window's pixel density. Recomputed each cycle so it tracks a window
         * moved to a display of a different density.
         */
        affine::extents2d const scale = pixel_density(window_id);
        SDL_Event event;
        /**
         * Window events carry the ID of the window they relate to. In a
         * single-window application anything other than our own window is
         * unexpected, so warn and ignore it rather than acting on it.
         */
        auto const from_my_window = [this, &event]() {
            bool const mine = (event.window.windowID == window_id);
            if (not mine) {
                planet::log::warning(
                        "Got a window event for a window ID that isn't mine. Mine",
                        window_id, "event", event.window.windowID);
            }
            return mine;
        };
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
                last_mouse_pos = {
                        event.motion.x * scale.width,
                        event.motion.y * scale.height};
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
                last_mouse_pos = {
                        event.motion.x * scale.width,
                        event.motion.y * scale.height};
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
                last_mouse_pos = {
                        event.motion.x * scale.width,
                        event.motion.y * scale.height};
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
                if (from_my_window()) { events.quit.push({}); }
                break;

            case SDL_EVENT_WINDOW_FOCUS_LOST:
                if (from_my_window()) {
                    events.focus.push(events::window_focus::lost);
                }
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                if (from_my_window()) {
                    events.focus.push(events::window_focus::gained);
                }
                break;

            /**
             * The pointer crossing the window boundary is independent of
             * keyboard focus -- it can leave a window that stays focused, and
             * the window can lose focus with the pointer still inside -- so
             * these are reported separately rather than as focus changes.
             */
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                if (from_my_window()) {
                    events.pointer.push(events::window_pointer::enter);
                }
                break;
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                if (from_my_window()) {
                    events.pointer.push(events::window_pointer::leave);
                }
                break;

            /**
             * Window position and size are reported in logical points -- the
             * same units the saved window geometry round-trips through -- so
             * they are used directly without the pixel density scaling the
             * pointer events need.
             */
            case SDL_EVENT_WINDOW_MOVED:
                if (from_my_window()) {
                    events.position.push(
                            affine::point2d{
                                    float(event.window.data1),
                                    float(event.window.data2)});
                }
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                if (from_my_window()) {
                    events.resize.push(
                            {.transition = events::window_resize::change,
                             .extents = affine::extents2d{
                                     float(event.window.data1),
                                     float(event.window.data2)}});
                }
                break;
            /**
             * Minimise, maximise and full screen are state changes that do not
             * themselves carry a new content size (a `SDL_EVENT_WINDOW_RESIZED`
             * follows when the size actually changes), so the extents are left
             * unset.
             */
            case SDL_EVENT_WINDOW_MINIMIZED:
                if (from_my_window()) {
                    events.resize.push(
                            {.transition = events::window_resize::minimise});
                }
                break;
            case SDL_EVENT_WINDOW_MAXIMIZED:
                if (from_my_window()) {
                    events.resize.push(
                            {.transition = events::window_resize::maximise});
                }
                break;
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                if (from_my_window()) {
                    events.resize.push(
                            {.transition = events::window_resize::full_screen});
                }
                break;

            default: break;
            }
        }
        co_await sdl.io.sleep(30ms);
    }
}
