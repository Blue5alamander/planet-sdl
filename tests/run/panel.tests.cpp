#include <planet/sdl/renderer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("panel");


    auto const h1 = suite.test("hierarchy 1", [](auto) {
        planet::sdl::panel p, pc;
        p.viewport.reflect_y();
        p.add_child(pc, {3, 4}, {5, 6});
    });
    auto const h2 = suite.test("hierarchy 2", [](auto) {
        planet::sdl::panel p1, p3;
        {
            planet::sdl::panel p2;
            p1.add_child(p2, {3, 4}, {5, 6});
            p2.add_child(p3, {1, 2}, {8, 9});
        }
    });


}
