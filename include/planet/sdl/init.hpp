#pragma once


#include <planet/sdl/ttf.hpp>

#include <felspar/io.hpp>


namespace planet::sdl {


    class init final {
      public:
        init(felspar::io::warden &w);
        ~init();

        felspar::io::warden &io;

        template<typename F, typename... Args>
        felspar::coro::task<int> run(felspar::io::warden &, F f, Args... args) {
            co_return co_await f(*this, std::forward<Args>(args)...);
        }
    };


    template<typename F, typename... Args>
    inline int co_main(F f, Args... args) {
        felspar::io::poll_warden w;
        init sdl{w};
        ttf text{sdl};
        return w.run<int, init, F, Args...>(
                sdl, &init::run, std::forward<F>(f),
                std::forward<Args>(args)...);
    }


}
