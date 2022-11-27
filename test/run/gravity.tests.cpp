#include <planet/sdl/ui/box.hpp>
#include <felspar/test.hpp>


namespace {


    constexpr planet::affine::extent2d unitbox{{0, 0}, {1, 1}};

    auto const gh = felspar::testsuite(
            "gravity/horizontal",
            [](auto check) {
                auto const v = planet::sdl::ui::within({}, unitbox, unitbox);
                check(v.top_left.x()) == 0;
                check(v.top_left.y()) == 0;
                check(v.bottom_right.x()) == 1;
                check(v.bottom_right.y()) == 1;
            },
            [](auto check) {
                auto const v =
                        planet::sdl::ui::within({}, {{2, 3}, {6, 8}}, unitbox);
                check(v.top_left.x()) == 2;
                check(v.top_left.y()) == 3;
                check(v.bottom_right.x()) == 6;
                check(v.bottom_right.y()) == 8;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::left, {{2, 3}, {6, 8}},
                        unitbox);
                check(v.top_left.x()) == 2;
                check(v.top_left.y()) == 3;
                check(v.bottom_right.x()) == 3;
                check(v.bottom_right.y()) == 8;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::right, {{2, 3}, {6, 8}},
                        unitbox);
                check(v.top_left.x()) == 5;
                check(v.top_left.y()) == 3;
                check(v.bottom_right.x()) == 6;
                check(v.bottom_right.y()) == 8;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::right
                                | planet::sdl::ui::gravity::left,
                        {{2, 3}, {6, 8}}, unitbox);
                check(v.top_left.x()) == 3.5f;
                check(v.top_left.y()) == 3;
                check(v.bottom_right.x()) == 4.5f;
                check(v.bottom_right.y()) == 8;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::right
                                | planet::sdl::ui::gravity::left,
                        unitbox, {{2, 3}, {6, 8}});
                check(v.top_left.x()) == -1.5f;
                check(v.top_left.y()) == 0;
                check(v.bottom_right.x()) == 2.5f;
                check(v.bottom_right.y()) == 1;
            });

    auto const gv = felspar::testsuite(
            "gravity/vertical",
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::top, {{2, 3}, {6, 8}},
                        unitbox);
                check(v.top_left.x()) == 2;
                check(v.top_left.y()) == 3;
                check(v.bottom_right.x()) == 6;
                check(v.bottom_right.y()) == 4;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::bottom, {{2, 3}, {6, 8}},
                        unitbox);
                check(v.top_left.x()) == 2;
                check(v.top_left.y()) == 7;
                check(v.bottom_right.x()) == 6;
                check(v.bottom_right.y()) == 8;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::top
                                | planet::sdl::ui::gravity::bottom,
                        {{2, 3}, {6, 8}}, unitbox);
                check(v.top_left.x()) == 2;
                check(v.top_left.y()) == 5;
                check(v.bottom_right.x()) == 6;
                check(v.bottom_right.y()) == 6;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::top
                                | planet::sdl::ui::gravity::bottom,
                        unitbox, {{2, 3}, {6, 8}});
                check(v.top_left.x()) == 0;
                check(v.top_left.y()) == -2;
                check(v.bottom_right.x()) == 1;
                check(v.bottom_right.y()) == 3;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::top
                                | planet::sdl::ui::gravity::bottom
                                | planet::sdl::ui::gravity::right
                                | planet::sdl::ui::gravity::left,
                        {{2, 3}, {6, 8}}, unitbox);
                check(v.top_left.x()) == 3.5f;
                check(v.top_left.y()) == 5;
                check(v.bottom_right.x()) == 4.5f;
                check(v.bottom_right.y()) == 6;
            },
            [](auto check) {
                auto const v = planet::sdl::ui::within(
                        planet::sdl::ui::gravity::top
                                | planet::sdl::ui::gravity::bottom
                                | planet::sdl::ui::gravity::right
                                | planet::sdl::ui::gravity::left,
                        unitbox, {{2, 3}, {6, 8}});
                check(v.top_left.x()) == -1.5f;
                check(v.top_left.y()) == -2;
                check(v.bottom_right.x()) == 2.5f;
                check(v.bottom_right.y()) == 3;
            });


}
