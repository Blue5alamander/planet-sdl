#include <planet/sdl.hpp>


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

    felspar::coro::task<void> run() { co_await sdl.io.sleep(3s); }
    planet::sdl::event_loop loop{sdl, window, *this, &ui::run};

    felspar::coro::stream<planet::sdl::renderer::frame> renderer() {
        while (true) {
            window.renderer.colour(5, 5, 5);
            window.renderer.clear();
            // TODO Draw UI
            co_yield window.renderer.present();
            co_await sdl.io.sleep(50ms);
        }
    }
};


/// ## Main


int main(int argc, char const *argv[]) {
    return planet::sdl::co_main(
            [](planet::sdl::init &sdl, int,
               char const *argv[]) -> felspar::coro::task<int> {
                co_return co_await ui{sdl, {argv[0]}}.loop.run();
            },
            argc, argv);
}
