#pragma once


#include <planet/log.hpp>
#include <planet/serialise/forward.hpp>
#include <planet/sdl/ttf.hpp>

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


        configuration(std::string_view appname);
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
         * persist. There are specific files and folders below for configuration, logs and game saves.
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
        std::optional<std::filesystem::path> log_filename;
        std::ofstream logfile;


        /// ### User's configuration
        log::level log_level = log::level::debug;
        bool auto_remove_log_files = true;
    };
    void save(serialise::save_buffer &, configuration const &);
    void save(serialise::load_buffer &, configuration &);


    /// ## Engine initialisation
    class init final {
      public:
        /// Pass the application name which is used to generate the
        /// configuration paths
        init(felspar::io::warden &w, std::string_view appname);
        ~init();

        configuration config;
        felspar::io::warden &io;

        template<typename F, typename... Args>
        felspar::coro::task<int> run(felspar::io::warden &, F f, Args... args) {
            co_return co_await f(*this, std::forward<Args>(args)...);
        }
    };


    /// ## Engine set up
    /// This will set up and configure Planet
    template<typename F, typename... Args>
    inline int co_main(F f, std::string_view appname, Args... args) {
        felspar::io::poll_warden w;
        init sdl{w, appname};
        ttf text{sdl};
        return w.run<int, init, F, Args...>(
                sdl, &init::run, std::forward<F>(f),
                std::forward<Args>(args)...);
    }


}
