#include <planet/sdl/ui/text.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("text", [](auto check) {
        check(planet::sdl::ui::text::identify_words("").empty()) == true;
    });


}
