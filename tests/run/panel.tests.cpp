#include <planet/sdl/renderer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("panel");


    auto const d = suite.test("default", [](auto) { planet::sdl::panel p; });


}
