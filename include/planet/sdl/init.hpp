#pragma once


#include <planet/audio/channel.hpp>
#include <planet/folders.hpp>
#include <planet/log.hpp>
#include <planet/serialise/forward.hpp>
#include <planet/telemetry/counter.hpp>
#include <planet/sdl/ttf.hpp>
#include <planet/version.hpp>

#include <felspar/io/warden.poll.hpp>


namespace planet::sdl {


    /// ## SDL initialisation flags
    enum class initialise : std::uint32_t {
        audio = 0x00000010u, ///< SDL_INIT_AUDIO
        video = 0x00000020u, ///< SDL_INIT_VIDEO
    };
    [[nodiscard]] constexpr auto
            operator|(initialise const lhs, initialise const rhs)
                    -> initialise {
        return static_cast<initialise>(
                static_cast<std::uint32_t>(lhs)
                | static_cast<std::uint32_t>(rhs));
    }


    /// ## Engine configuration
    /**
     * The user's preferences — logging, audio and some persistent telemetry.
     * All other game configuration is managed by the game itself. The file
     * locations these preferences are saved to and loaded from are handled by
     * `files`.
     */
    struct configuration final {
        static constexpr std::string_view box{"_p:sdl:config"};


        /// ### Creation
        configuration(log::level = log::level::debug);
        /**
         * Applies `level` to the global `planet::log::active` straight away so
         * logging is filtered from the very first message. Pass a higher level
         * (for example in tests) to suppress the lower-level messages.
         */


        /// ### User's configuration
        log::level log_level;
        bool save_logs_to_file = true;
        bool auto_remove_log_files = true;
        bool upload_performance_data = true;


        /// ### Audio configuration
        std::optional<std::string> audio_device_name = {};
        audio::channel master_volume{audio::dB_gain{-9}};
        audio::channel music_volume{audio::dB_gain{-15}};
        audio::channel sfx_volume{audio::dB_gain{-3}};

        /// #### Blocks of audio latency injected ahead of the device
        std::size_t audio_latency_injected_block_count = 2;
        /**
         * The mixer pre-renders this many blocks ahead of the SDL callback (the
         * mixer ring depth / `driver::block_count`), trading output latency for
         * resilience against producer-thread scheduling jitter. A track handed
         * to a mixer becomes audible `block_size * this` samples later, on top
         * of the device's own buffering. Must be in `[1,
         * mixer::max_ring_depth]` or the driver logs `critical`.
         */


        /// ### Performance counters
        telemetry::counter times_exited = {"planet_sdl_config_times_exited", 0};
        telemetry::counter times_loaded = {"planet_sdl_config_times_loaded", 1};
    };
    void save(serialise::save_buffer &, configuration const &);
    void load(serialise::load_buffer &, configuration &);


    /// ## Engine file locations
    /**
     * The folders and files that the engine reads from and writes to. These are
     * calculated when the object is created, but may be altered later using the
     * `set_game_folder` method if the initial location is no good (this can
     * happen on platforms like Android where the correct writeable folder is
     * known too late).
     *
     * File logging will be automatically turned on when `save_logs_to_file` is
     * set in the `configuration`.
     */
    struct files final {
        /// ### Creation
        files(version const &, configuration const &);
        ~files();


        /// #### Game folder
        std::filesystem::path game_folder;
        /**
         *
         * The game should use this folder to save any data that it may need to
         * persist. There are specific files and folders below for
         * configuration, logs and game saves.
         */

        /// #### Changing the game folder
        void set_game_folder(std::filesystem::path);
        /**
         * On some platforms it may not be possible to reliably determine the
         * folder name at start up. For those this method should be called so
         * that the folder structure can be determined.
         */

        /// #### Configuration file name
        std::filesystem::path config_filename;
        /// #### Save game folder
        std::filesystem::path save_folder;

        /// #### Logging
        std::filesystem::path log_folder;
        std::optional<std::filesystem::path> log_filename, perf_filename;
        std::ofstream logfile, perfile;


      private:
        configuration const &config;
    };


    /// ## Engine initialisation
    class init final {
      public:
        /**
         * Pass the application name which is used to generate the configuration
         * paths
         */
        init(felspar::io::warden &w,
             version const &,
             initialise const subsystems = initialise::video
                     | initialise::audio);
        ~init();

        configuration config;
        sdl::files files;
        felspar::io::warden &io;


        template<typename F, typename... Args>
        felspar::coro::task<int> run(felspar::io::warden &, F f, Args... args) {
            co_return co_await f(*this, std::forward<Args>(args)...);
        }


        std::span<std::optional<std::string> const>
                audio_devices() const noexcept {
            return audio_device_list;
        }


      private:
        std::vector<std::optional<std::string>> audio_device_list;
    };


    /// ## Engine set up
    /// This will set up and configure Planet
    template<typename F, typename... Args>
    inline int co_main(F f, planet::version const &version, Args... args) {
        felspar::io::poll_warden w;
        init sdl{w, version};
        ttf text{sdl};
        return w.run<int, init, F, Args...>(
                sdl, &init::run, std::forward<F>(f),
                std::forward<Args>(args)...);
    }


}
