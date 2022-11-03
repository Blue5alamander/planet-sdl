#include <planet/sdl/renderer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("panel");


    auto const d = suite.test("default", [](auto check) {
        planet::sdl::panel p, pc;
        p.viewport.reflect_y();
        p.add_child(pc, {3, 4}, {5, 6});
    });


}
