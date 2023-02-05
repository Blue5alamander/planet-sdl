#include <planet/sdl.hpp>


using namespace std::literals;


/// # Mouse handling


/// ## User Interface

struct ui {
    ui(planet::sdl::init &s, std::filesystem::path exe)
    : sdl{s}, assets{std::move(exe)},
    window{sdl, "Planet SDL mouse example", SDL_WINDOW_FULLSCREEN_DESKTOP} {}

        planet::sdl::init &sdl;
        planet::asset_manager assets;
        planet::sdl::window window;

    felspar::coro::task<int> run() {
        co_await sdl.io.sleep(3s);
        co_return 0;
    }
};


/// ## Main



int main(int argc, char const *argv[]) {
    return planet::sdl::co_main(
            [](planet::sdl::init &sdl, int,
               char const *argv[]) -> felspar::coro::task<int> {
                co_return co_await ui{sdl, {argv[0]}}.run();
            },
            argc, argv);
}
