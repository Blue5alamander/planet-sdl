#pragma once


#include <planet/audio/channel.hpp>
#include <planet/folders.hpp>
#include <planet/log.hpp>
#include <planet/serialise/forward.hpp>
#include <planet/telemetry/counter.hpp>
#include <planet/sdl/ttf.hpp>
#include <planet/version.hpp>

#include <felspar/io.hpp>

#include <fstream>


namespace planet::sdl {


    /// ## Engine configuration
    /**
     * This configuraiton is only used for the save folder and the configuration
     * save path. All other game configuration is managed by the game itself.
     *
     * File logging will be automatically turned on.
     */
    struct configuration final {
        static constexpr std::string_view box{"_p:sdl:config"};


        /// ### Creation
        configuration(version const &);
        ~configuration();


        /// ### File configuration
        /**
         * File configuration is calculated when the configuration object is
         * created, but may be altered later using the `set_game_folder` method
         * later on if the initial configuration is no good (this can happen on
         * platforms like Android where the correct writeable folder is known
         * too late).
         */

        /// #### Game folder
        /**
         *
         * The game should use this folder to save any data that it may need to
         * persist. There are specific files and folders below for
         * configuration, logs and game saves.
         */
        std::filesystem::path game_folder;

        /// #### Changing the game folder
        /**
         * On some platforms it may not be possible to reliably determine the
         * folder name at start up. For those this method should be called so
         * that the folder structure can be determined.
         */
        void set_game_folder(std::filesystem::path);

        /// #### Configuration file name
        std::filesystem::path config_filename;
        /// #### Save game folder
        std::filesystem::path save_folder;

        /// #### Logging
        std::filesystem::path log_folder;
        std::optional<std::filesystem::path> log_filename, perf_filename;
        std::ofstream logfile, perfile;


        /// ### User's configuration
        log::level log_level = log::level::debug;
        bool save_logs_to_file = true;
        bool auto_remove_log_files = true;
        bool upload_performance_data = true;


        /// ### Audio configuration
        std::optional<std::string> audio_device_name = {};
        audio::channel master_volume{audio::dB_gain{-9}};
        audio::channel music_volume{audio::dB_gain{-15}};
        audio::channel sfx_volume{audio::dB_gain{-3}};


        /// ### Performance counters
        telemetry::counter times_exited = {"planet_sdl_config_times_exited", 0};
        telemetry::counter times_loaded = {"planet_sdl_config_times_loaded", 1};
    };
    void save(serialise::save_buffer &, configuration const &);
    void load(serialise::load_buffer &, configuration &);


    /// ## Engine initialisation
    class init final {
      public:
        /**
         * Pass the application name which is used to generate the configuration
         * paths
         */
        init(felspar::io::warden &w, version const &);
        ~init();

        configuration config;
        felspar::io::warden &io;


        template<typename F, typename... Args>
        felspar::coro::task<int> run(felspar::io::warden &, F f, Args... args) {
            co_return co_await f(*this, std::forward<Args>(args)...);
        }


        std::span<std::string const> audio_devices() const noexcept {
            return audio_device_list;
        }


      private:
        std::vector<std::string> audio_device_list;
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
