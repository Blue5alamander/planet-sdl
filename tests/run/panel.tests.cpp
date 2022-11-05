#include <planet/ostream.hpp>
#include <planet/sdl/renderer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("panel");


    auto const h1 = suite.test("hierarchy 1", [](auto check) {
        planet::sdl::panel p, pc;
        p.add_child(pc, {3, 4}, {5, 6});
        check(p.mouse_click.latest()).is_falsey();
        check(pc.mouse_click.latest()).is_falsey();

        p.mouse_click.push({2, 3});
        check(p.mouse_click.latest()) == planet::affine::point2d{2, 3};
        check(pc.mouse_click.latest()).is_falsey();

        p.mouse_click.push({4, 6});
        check(p.mouse_click.latest()) == planet::affine::point2d{4, 6};
        check(pc.mouse_click.latest()).is_truthy();
        auto const click = *pc.mouse_click.latest();
        check(click.x()) == 1;
        check(click.y()) == 2;
    });
    auto const h2 = suite.test("hierarchy 2", [](auto check) {
        planet::sdl::panel p1, p3;
        check(p1.mouse_click.latest()).is_falsey();
        check(p3.mouse_click.latest()).is_falsey();
        {
            planet::sdl::panel p2;
            check(p2.mouse_click.latest()).is_falsey();
            p1.add_child(p2, {3, 4}, {15, 16});
            p2.add_child(p3, {1, 2}, {8, 9});

            p3.mouse_click.push({1, 2});
            check(p3.mouse_click.latest()) == planet::affine::point2d{1, 2};

            p2.mouse_click.push({1, 2});
            check(p2.mouse_click.latest()) == planet::affine::point2d{1, 2};
            check(p3.mouse_click.latest()) == planet::affine::point2d{0, 0};

            p1.mouse_click.push({5, 8});
            check(p1.mouse_click.latest()) == planet::affine::point2d{5, 8};
            check(p2.mouse_click.latest()) == planet::affine::point2d{2, 4};
            check(p3.mouse_click.latest()) == planet::affine::point2d{1, 2};
        }
        p1.mouse_click.push({5, 8});
        check(p1.mouse_click.latest()) == planet::affine::point2d{5, 8};
        /// p3 doesn't get the click
        check(p3.mouse_click.latest()) == planet::affine::point2d{1, 2};
    });


}
