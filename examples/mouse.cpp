#include <planet/sdl.hpp>

#include <felspar/coro/start.hpp>


using namespace std::literals;


/// # Mouse handling


/// ## User Interface


struct ui {
    ui(planet::sdl::init &s, std::filesystem::path exe)
    : sdl{s}, assets{std::move(exe)} {}

    planet::sdl::init &sdl;
    planet::asset_manager assets;

    planet::sdl::window window{
            sdl, "Planet SDL mouse example", SDL_WINDOW_FULLSCREEN_DESKTOP};

    felspar::coro::task<int> run() {
        co_await sdl.io.sleep(3s);
        co_return 0;
    }
    planet::sdl::event_loop loop{sdl, window};

    felspar::coro::stream<planet::sdl::renderer::frame> renderer() {
        while (true) {
            window.renderer.colour(5, 5, 5);
            window.renderer.clear();
            // TODO Draw UI
            co_yield window.renderer.present();
            co_await sdl.io.sleep(50ms);
        }
    }

    felspar::coro::starter<> coros;
    std::vector<std::string> raw_mouse_events, mouse_clicks;
    felspar::coro::task<void> save_raw_mouse_events();
    felspar::coro::task<void> save_mouse_clicks();
};


/// ## Main


int main(int argc, char const *argv[]) {
    return planet::sdl::co_main(
            [](planet::sdl::init &sdl, int,
               char const *argv[]) -> felspar::coro::task<int> {
                co_return co_await ui{sdl, {argv[0]}}.run();
            },
            "planet/mouse-example-planet-sdl", argc, argv);
}
