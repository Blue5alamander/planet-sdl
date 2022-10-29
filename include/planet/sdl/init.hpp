#pragma once


#include <planet/sdl/ttf.hpp>

#include <felspar/coro/task.hpp>


namespace planet::sdl {


    class init final {
      public:
        init();
        ~init();
    };


    template<typename F, typename... Args>
    felspar::coro::task<int> co_main(F f, Args... args) {
        init sdl{};
        ttf text{sdl};
        co_return co_await f(sdl, std::forward<Args>(args)...);
    }


}
