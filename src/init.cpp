#include <planet/sdl/init.hpp>

#include <planet/folders.hpp>
#include <planet/log.hpp>

#include <iostream>

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
planet::sdl::configuration::configuration(planet::version const &version) {
    log::active.store(log_level);
    std::filesystem::path home =
            base_storage_folder() / version.application_folder;
    set_game_folder(std::move(home));
}


planet::sdl::configuration::~configuration() {
    planet::log::stop_thread();
    logfile.close();
    auto const logs = log::counters::current();
    if (auto_remove_log_files and log_filename and not logs.error) {
        std::cerr << "Removing log file " << *log_filename << '\n';
        std::error_code ec;
        std::filesystem::remove(*log_filename, ec);
        if (ec) {
            std::cerr << "Error removing log file " << *log_filename << " "
                      << ec.message() << '\n';
        }
    } else {
        std::cerr << "Keeping log file\n";
    }
}


void planet::sdl::configuration::set_game_folder(std::filesystem::path path) {
    config_filename = path / "configuration";
    log_folder = path / "logs";
    save_folder = path / "saves";
    game_folder = std::move(path);

    if (try_make_folder(log_folder)) {
        auto const basename = std::to_string(
                std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count());
        /**
         * These configuration items won't yet hold the values from the config
         * file because that won't get loaded until much later...
         */
        if (save_logs_to_file) {
            log_filename = log_folder / (basename + ".plog");
            logfile.open(*log_filename, std::ios::binary);
            planet::log::log_output.store(&logfile);
        }
        log::write_file_headers();
        log::info(
                "Game path", game_folder, "configuration file", config_filename,
                "save folder", save_folder, "log file", log_filename);
    } else {
        log::info(
                "Game path", game_folder, "configuration file", config_filename,
                "save folder", save_folder, "log folder", log_folder);
    }
    try_make_folder(save_folder);
}


void planet::sdl::save(serialise::save_buffer &sb, configuration const &c) {
    sb.save_box(
            c.box, c.log_level, c.auto_remove_log_files, c.master_volume,
            c.music_volume, c.sfx_volume, c.audio_device_name,
            c.save_logs_to_file, c.upload_performance_data);
    telemetry::save_performance(sb, c.times_exited, c.times_loaded);
}
void planet::sdl::load(serialise::load_buffer &lb, configuration &c) {
    auto b = planet::serialise::expect_box(lb);
    b.lambda(c.box, [&]() {
        b.fields(c.log_level, c.auto_remove_log_files);
        if (b.content.empty()) { return; }
        b.fields(c.master_volume, c.music_volume, c.sfx_volume);
        if (b.content.empty()) { return; }
        b.fields(c.audio_device_name);
        if (b.content.empty()) { return; }
        b.fields(c.save_logs_to_file, c.upload_performance_data);
    });
    if (not lb.empty()) {
        telemetry::load_performance(lb, c.times_exited, c.times_loaded);
    }
    log::active.store(c.log_level);
    /**
     * TODO Probably we need to deal with changes to the log and pref save file
     * stuff now that the configuration has been loaded
     */
}


/// ## `planet::sdl::init`


planet::sdl::init::init(felspar::io::warden &w, planet::version const &version)
: config{version}, io{w} {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    static constexpr int iscapture = false;
    auto const device_count = SDL_GetNumAudioDevices(iscapture);
    audio_device_list.push_back({});
    if (device_count < 1) {
        /**
         * Apparently this can happen, but that doesn't mean that SDL will fail
         * to be able to open one, it just means it doesn't know what is
         * available. <https://wiki.libsdl.org/SDL2/SDL_GetNumAudioDevices>
         */
    } else {
        for (int device = 0; device < device_count; ++device) {
            char const *const dn = SDL_GetAudioDeviceName(device, iscapture);
            bool const is_empty = (dn == nullptr or *dn == 0);
            if (not is_empty) { audio_device_list.push_back(dn); }
        }
    }
    planet::log::debug(
            "Audio devices found", device_count, "device list",
            audio_device_list);
}


planet::sdl::init::~init() {
    SDL_Quit();
#ifndef __ANDROID__
    /**
     * `SDL_Quit` does [not clean up its thread local
     * data](https://github.com/libsdl-org/SDL/issues/6200). On Android it seems
     * that we should expect to re-enter SDL after quitting as the process
     * doesn't go away, so we don't do this clean up on Android.
     */
    SDL_TLSCleanup();
#endif
}
