#pragma once


#include <planet/sdl/ttf.hpp>

#include <felspar/io.hpp>


namespace planet::sdl {


    /// ## Engine configuration
    /**
     * This configuraiton is only used for the save folder and the configuration
     * save path. All other game configuration is managed by the game itself.
     */
    struct configuration final {
        configuration(std::string_view appname);

        /// ### The folder that the game has to use for any data that it may
        /// need to persist
        std::filesystem::path game_folder;
        /**
         * On some platforms it may not be possible to reliably determine the
         * folder name at start up. For those this method should be called so
         * that the folder structure can be determined.
         */
        void set_game_folder(std::filesystem::path);

        std::filesystem::path config_filename;
        std::filesystem::path save_folder;
    };


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
