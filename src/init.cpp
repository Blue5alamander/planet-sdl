#include <planet/sdl/init.hpp>

#include <planet/folders.hpp>
#include <planet/log.hpp>

#include <SDL.h>


/// ## `planet::sdl::configuration`


namespace {
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
planet::sdl::configuration::configuration(std::string_view const appname) {
    log::active.store(log_level);
    std::filesystem::path home = base_storage_folder() / appname;
    set_game_folder(std::move(home));
}


planet::sdl::configuration::~configuration() {
    auto const logs = log::counters::current();
    if (auto_remove_log_files and log_filename and not logs.error) {
        std::error_code ec;
        std::filesystem::remove(*log_filename, ec);
    }
}


void planet::sdl::configuration::set_game_folder(std::filesystem::path path) {
    config_filename = path / "configuration";
    log_folder = path / "logs";
    save_folder = path / "saves";
    game_folder = std::move(path);

    if (try_make_folder(log_folder)) {
        log_filename = log_folder
                / (std::to_string(
                           std::chrono::duration_cast<std::chrono::seconds>(
                                   std::chrono::system_clock::now()
                                           .time_since_epoch())
                                   .count())
                   + ".plog");
        logfile.open(*log_filename, std::ios::binary);
        planet::log::output.store(&logfile);
        log::write_log_file_header();
        log::info(
                "Game path", game_folder, "configuration file", config_filename,
                "save folder", save_folder, "log file", *log_filename);
    } else {
        log::info(
                "Game path", game_folder, "configuration file", config_filename,
                "save folder", save_folder, "log folder", log_folder);
    }
    try_make_folder(save_folder);
}


void planet::sdl::save(serialise::save_buffer &sb, configuration const &c) {
    sb.save_box(c.box, c.log_level, c.auto_remove_log_files);
    telemetry::save_performance(sb, c.times_exited, c.times_loaded);
}
void planet::sdl::load(serialise::load_buffer &lb, configuration &c) {
    lb.load_box(c.box, c.log_level, c.auto_remove_log_files);
    if (not lb.empty()) {
        telemetry::load_performance(lb, c.times_exited, c.times_loaded);
    }
    log::active.store(c.log_level);
}


/// ## `planet::sdl::init`


planet::sdl::init::init(felspar::io::warden &w, std::string_view an)
: config{an}, io{w} {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}


planet::sdl::init::~init() { SDL_Quit(); }
