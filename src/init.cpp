#include <planet/sdl/init.hpp>

#include <planet/log.hpp>

#include <SDL.h>


/// ## `planet::sdl::configuration`


namespace {
    char const *safe_getenv(char const *env) {
        char const *value = std::getenv(env);
        if (value) {
            return value;
        } else {
            return "";
        }
    }
}
planet::sdl::configuration::configuration(std::string_view appname) {
    std::filesystem::path home;
#ifdef _WIN32
    home = safe_getenv("APPDATA");
#else
    home = safe_getenv("HOME");
#endif
    if (home.empty()) {
        home = std::filesystem::current_path();
    } else {
#ifdef _WIN32
        home /= appname;
#else
        home /= ".local/share";
        home /= appname;
#endif
    }
    set_game_folder(std::move(home));
}


void planet::sdl::configuration::set_game_folder(std::filesystem::path path) {
    config_filename = path / "configuration";
    save_folder = path / "saves";
    /// Try to create the directory. This may not be possible on first
    /// initialisation on some platforms and there's not a lot we can do about a
    /// bad path anyway, so just ignore any errors.
    std::error_code ec;
    std::filesystem::create_directories(save_folder, ec);
    if (ec) {
        log::warning(
                "Could not create save file directory", save_folder, "error",
                ec.message());
    }
}


/// ## `planet::sdl::init`


planet::sdl::init::init(felspar::io::warden &w, std::string_view an)
: config{an}, io{w} {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}


planet::sdl::init::~init() { SDL_Quit(); }
