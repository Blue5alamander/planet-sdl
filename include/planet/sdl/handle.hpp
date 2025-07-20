#pragma once


#include <utility>


namespace planet::sdl {


    /// ## An SDL handle
    template<typename T, void (*F)(T *)>
    class handle final
    /**
     * Stores a pointer to the resource `T` and frees it using the freeing
     * function `F`.
     *
     * In practice many of the freeing functions will behave properly when
     * passed a `nullptr`, but for the sake of not having to look them all up,
     * we perform a check in the destructor anyway.
     */
    {
        T *object = nullptr;

      public:
        handle() {}
        handle(T *t) : object{t} {}
        ~handle() { reset(); }


        /// ### A handle is movable, but not copyable
        handle(handle &&h) : object{std::exchange(h.object, nullptr)} {}
        handle(handle const &) = delete;

        handle &operator=(handle &&h) {
            std::swap(object, h.object);
            return *this;
        }
        handle &operator=(handle const &) = delete;


        /// ### Access to the underlying pointer
        T *get() const noexcept { return object; }
        T *operator->() const { return object; }


        /// ### Force reset of the handle
        void reset() {
            if (object) { F(std::exchange(object, nullptr)); }
        }
    };


}
