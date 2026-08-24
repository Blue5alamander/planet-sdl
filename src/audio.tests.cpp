#include <planet/sdl/audio.hpp>

#include <planet/audio/backend.hpp>

#include <felspar/test.hpp>

#include <concepts>


namespace {


    auto const suite = felspar::testsuite("audio");


    /**
     * `planet::sdl::audio_output` is one implementation of the shared backend
     * interface, so it must be reachable and destructible through a
     * `planet::audio::backend` reference. This is checked at compile time
     * because constructing an `audio_output` opens a real playback device,
     * which CI has none of — the reseat's runtime behaviour is covered by the
     * `planet::audio::output` tests it now delegates to.
     */
    static_assert(
            std::derived_from<planet::sdl::audio_output, planet::audio::backend>);
    static_assert(std::has_virtual_destructor_v<planet::audio::backend>);


    auto const conformance = suite.test("conformance", [](auto check) {
        check(std::derived_from<
                planet::sdl::audio_output, planet::audio::backend>)
                == true;
    });


}
