#include <planet/sdl/file_dialog.hpp>

#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("sdl-file-dialog");


    auto const decode = suite.test(
            "first_selected_path",
            [](auto check) {
                /// A `nullptr` list is SDL's way of reporting an error.
                check(planet::sdl::first_selected_path(nullptr).has_value())
                        == false;
            },
            [](auto check) {
                /// A pointer to `nullptr` means the user chose nothing.
                char const *const cancelled[] = {nullptr};
                check(planet::sdl::first_selected_path(cancelled).has_value())
                        == false;
            },
            [](auto check) {
                char const *const chosen[] = {"project.bw-project", nullptr};
                auto const path = planet::sdl::first_selected_path(chosen);
                check(path.has_value()) == true;
                check(path.value().string()) == "project.bw-project";
            },
            [](auto check) {
                /// Only the first entry is taken for a single-select dialog.
                char const *const many[] = {"first", "second", nullptr};
                check(planet::sdl::first_selected_path(many).value().string())
                        == "first";
            });


}
