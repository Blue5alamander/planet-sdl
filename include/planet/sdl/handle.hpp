#pragma once


#include <utility>


namespace planet::sdl {


    /// An SDL handle stores a pointer to the resource `T` and frees it using
    /// the freeing function `F`
    template<typename T, void (*F)(T *)>
    class handle final {
        T *object = nullptr;

      public:
        handle() {}
        handle(T *t) : object{t} {}
        handle(handle &&h) : object{std::exchange(h.object, nullptr)} {}
        handle(handle const &) = delete;
        ~handle() { reset(); }

        handle &operator=(handle &&h) {
            reset();
            object = std::exchange(h.object, nullptr);
        }
        handle &operator=(handle const &) = delete;

        operator T *() const noexcept { return object; }
        T *operator->() const { return object; }

        void reset() {
            if (object) { F(std::exchange(object, nullptr)); }
        }
    };


}
