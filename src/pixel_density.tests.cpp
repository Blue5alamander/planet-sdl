#include <planet/ostream.hpp>
#include <planet/sdl/pixel_density.hpp>

#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("sdl-pixel-density");


    auto const rectangle = suite.test(
            "window rectangle",
            [](auto check) {
                /**
                 * On an ordinary display the two coordinate spaces are the
                 * same, so the only thing that happens to a rectangle is being
                 * rounded into SDL's integers.
                 */
                auto const r = planet::sdl::window_rectangle(
                        {{15, 20}, planet::affine::extents2d{10, 3}},
                        {1.0f, 1.0f});

                check(r.x) == 15;
                check(r.y) == 20;
                check(r.w) == 10;
                check(r.h) == 3;
            },
            [](auto check) {
                /**
                 * The UI works in drawable pixels because the event loop
                 * scales pointer positions up by the density as they arrive,
                 * so reaching the window points SDL's own APIs take means
                 * dividing back down again. A field laid out at twice the
                 * coordinates on a Retina display sits in the same place.
                 */
                auto const r = planet::sdl::window_rectangle(
                        {{30, 40}, planet::affine::extents2d{20, 6}},
                        {2.0f, 2.0f});

                check(r.x) == 15;
                check(r.y) == 20;
                check(r.w) == 10;
                check(r.h) == 3;
            },
            [](auto check) {
                /**
                 * A rectangle that does not land on whole window points is
                 * rounded outwards, so what SDL is told about always covers
                 * the whole of what was laid out. Here the field runs from
                 * `7.5` to `12.5` points across and `10.25` to `11.75` down.
                 */
                auto const r = planet::sdl::window_rectangle(
                        {{15, 20.5f}, planet::affine::extents2d{10, 3}},
                        {2.0f, 2.0f});

                check(r.x) == 7;
                check(r.y) == 10;
                check(r.w) == 6;
                check(r.h) == 2;
            });


    auto const length = suite.test("window length", [](auto check) {
        /**
         * The caret offset SDL is handed alongside the area is measured in the
         * same window points, so it scales the same way.
         */
        check(planet::sdl::window_length(10.0f, 1.0f)) == 10;
        check(planet::sdl::window_length(10.0f, 2.0f)) == 5;
    });


}
