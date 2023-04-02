#include <planet/sdl/ui/text.hpp>
#include <felspar/test.hpp>
#include <felspar/memory/hexdump.hpp>


namespace {


    auto const suite = felspar::testsuite(
            "text",
            [](auto check) {
                std::string str{};
                check(planet::sdl::ui::text::identify_words(str).empty())
                        == true;
            },
            [](auto check, auto &log) {
                std::string str{"Hello World!"};
                auto const words = planet::sdl::ui::text::identify_words(str);
                felspar::memory::hexdump(
                        log, std::span{str.data(), str.size()});
                check(words.size()) == 2u;
                felspar::memory::hexdump(
                        log, std::span{words[0].data(), words[0].size()});
                check(words[0]) == "Hello";
                felspar::memory::hexdump(
                        log, std::span{words[1].data(), words[1].size()});
                check(words[1]) == "World!";
            },
            [](auto check, auto &log) {
                std::string str{"  Hello World!"};
                auto const words = planet::sdl::ui::text::identify_words(str);
                felspar::memory::hexdump(
                        log, std::span{str.data(), str.size()});
                check(words.size()) == 2u;
                felspar::memory::hexdump(
                        log, std::span{words[0].data(), words[0].size()});
                check(words[0]) == "Hello";
                felspar::memory::hexdump(
                        log, std::span{words[1].data(), words[1].size()});
                check(words[1]) == "World!";
            },
            [](auto check, auto &log) {
                std::string str{"Hello World!  "};
                auto const words = planet::sdl::ui::text::identify_words(str);
                felspar::memory::hexdump(
                        log, std::span{str.data(), str.size()});
                check(words.size()) == 2u;
                felspar::memory::hexdump(
                        log, std::span{words[0].data(), words[0].size()});
                check(words[0]) == "Hello";
                felspar::memory::hexdump(
                        log, std::span{words[1].data(), words[1].size()});
                check(words[1]) == "World!";
            });


}
