#include <planet/sdl/init.hpp>

#include <planet/folders.hpp>
#include <planet/log.hpp>
#include <planet/serialise/affine.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/sdl/sdl.hpp>

#include <felspar/exceptions/runtime_error.hpp>

#include <iostream>
#include <string>
#include <utility>


using namespace std::literals;


/// ## `planet::sdl::configuration`


planet::sdl::configuration::configuration(log::level const level)
: log_level{level} {
    log::active.store(log_level);
}


/// ## `planet::sdl::files`


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


planet::sdl::files::files(planet::version const &version, configuration const &c)
: config{c} {
    std::filesystem::path home =
            base_storage_folder() / version.application_folder;
    set_game_folder(std::move(home));
}


planet::sdl::files::~files() {
    planet::log::stop_thread();
    logfile.close();
    auto const logs = log::counters::current();
    if (config.auto_remove_log_files and log_filename and not logs.error) {
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


void planet::sdl::files::set_game_folder(std::filesystem::path path) {
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
        if (config.save_logs_to_file) {
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


/// ## `planet::sdl::window_mode`


std::string_view planet::sdl::to_string(window_mode const m) {
    switch (m) {
    case window_mode::windowed_fixed_size: return "windowed-fixed-size";
    case window_mode::windowed_resizable: return "windowed-resizable";
    case window_mode::full_screen_windowed: return "full-screen-windowed";
    case window_mode::full_screen_borderless: return "full-screen-borderless";
    }
    std::unreachable();
}
auto planet::sdl::window_mode_from_string(std::string_view const s)
        -> window_mode {
    if (s == "windowed" or s == "windowed-fixed-size") {
        /**
         * `"windowed"` is the legacy spelling from before the fixed/resizable
         * split and still loads as the fixed-size mode.
         */
        return window_mode::windowed_fixed_size;
    } else if (s == "windowed-resizable") {
        return window_mode::windowed_resizable;
    } else if (s == "full-screen-windowed") {
        return window_mode::full_screen_windowed;
    } else if (s == "full-screen-borderless") {
        return window_mode::full_screen_borderless;
    } else {
        throw felspar::stdexcept::runtime_error{
                "Cannot parse window_mode\nString: '" + std::string{s} + "'"};
    }
}
void planet::sdl::save(serialise::save_buffer &b, window_mode const m) {
    b.save_box(1, window_mode_box, to_string(m));
}
void planet::sdl::load(serialise::box &b, window_mode &m) {
    b.lambda(std::array{window_mode_box, "planet::sdl::window_mode"sv}, [&]() {
        if (b.version == 1) {
            std::string s;
            b.fields(s);
            m = window_mode_from_string(s);
        } else {
            b.throw_unsupported_version(1);
        }
    });
}


/// ## `planet::sdl::configuration` save and load


void planet::sdl::save(serialise::save_buffer &sb, configuration const &c) {
    sb.save_box(
            c.box, c.log_level, c.auto_remove_log_files, c.master_volume,
            c.music_volume, c.sfx_volume, c.audio_device_name,
            c.save_logs_to_file, c.upload_performance_data,
            c.audio_latency_injected_block_count, c.window_display_mode,
            c.window_extents, c.window_position);
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
        if (b.content.empty()) { return; }
        b.fields(c.audio_latency_injected_block_count);
        if (b.content.empty()) { return; }
        b.fields(c.window_display_mode, c.window_extents, c.window_position);
    });
    if (not lb.empty()) {
        telemetry::load_performance(lb, c.times_exited, c.times_loaded);
    }
    log::active.store(c.log_level);
}


/// ## `planet::sdl::init`


static_assert(
        static_cast<std::uint32_t>(planet::sdl::initialise::video)
        == SDL_INIT_VIDEO);
static_assert(
        static_cast<std::uint32_t>(planet::sdl::initialise::audio)
        == SDL_INIT_AUDIO);


namespace {
    /// ## Deleter for the SDL3 audio playback device list
    /**
     * `SDL_GetAudioPlaybackDevices` returns an array allocated by SDL that must
     * be released with `SDL_free`. That takes `void *`, but the
     * `planet::sdl::handle` deleter signature is `void(T *)`; this adapts the
     * two so the device list can be owned by a `handle` rather than freed by
     * hand.
     */
    void free_audio_device_list(SDL_AudioDeviceID *devices) noexcept {
        SDL_free(devices);
    }
}


planet::sdl::init::init(
        felspar::io::warden &w,
        planet::version const &version,
        initialise const subsystems)
: files{version, config}, io{w} {
    if (not SDL_Init(static_cast<std::uint32_t>(subsystems))) {
        throw felspar::stdexcept::runtime_error{
                std::string{"SDL_Init failed "} + SDL_GetError()};
    }
    audio_device_list.push_back({});
    /**
     * SDL3 hands back a freshly-allocated, 0-terminated array of playback
     * device IDs that we own until `SDL_free`, and looks each name up by ID
     * rather than by index. The array is held in a `handle` so it is released
     * even if the enumeration below throws.
     */
    int device_count = 0;
    handle<SDL_AudioDeviceID, free_audio_device_list> devices{
            SDL_GetAudioPlaybackDevices(&device_count)};
    if (devices.get() == nullptr or device_count < 1) {
        /**
         * Apparently this can happen, but that doesn't mean that SDL will fail
         * to be able to open one, it just means it doesn't know what is
         * available.
         */
    } else {
        for (int device = 0; device < device_count; ++device) {
            char const *const dn =
                    SDL_GetAudioDeviceName(devices.get()[device]);
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
    SDL_CleanupTLS();
#endif
}
