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
    bool try_make_folder(std::filesystem::path const &dir) {
        /**
         * Try to create the directory. This may not be possible on first
         * initialisation on some platforms and there's not a lot we can do
         * about a bad path anyway, so just ignore any errors.
         */
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            planet::log::warning(
                    "Could not create save file directory", dir, "error",
                    ec.message());
            return false;
        } else {
            return true;
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
    auto logname = path / "logs";
    game_folder = std::move(path);

    if (try_make_folder(logname)) {
        logname /=
                std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                                       std::chrono::system_clock::now()
                                               .time_since_epoch())
                                       .count())
                + ".plog";
        logfile.open(logname, std::ios::binary);
        planet::log::output.store(&logfile);
        log::info(
                "Game path", game_folder, "configuration file", config_filename,
                "save folder", save_folder, "log file", logname);
    } else {
        log::info(
                "Game path", game_folder, "configuration file", config_filename,
                "save folder", save_folder, "log folder", logname);
    }
    try_make_folder(save_folder);
}


/// ## `planet::sdl::init`


planet::sdl::init::init(felspar::io::warden &w, std::string_view an)
: config{an}, io{w} {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}


planet::sdl::init::~init() { SDL_Quit(); }
