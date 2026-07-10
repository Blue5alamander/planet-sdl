#include <planet/sdl/ttf.hpp>
#include <planet/ui/scale.hpp>

#include <felspar/exceptions/runtime_error.hpp>

#include <source_location>
#include <string>


namespace {


#if PLANET_SDL3
    /// Throw if an SDL TTF API call reports failure
    /**
     * SDL3's TTF calls (sizing, rendering and `TTF_Init`) return `bool` and
     * report failure as `false`.
     */
    void check_worked(
            bool const ok,
            std::source_location const &loc = std::source_location::current()) {
        if (not ok) [[unlikely]] {
            throw felspar::stdexcept::runtime_error{
                    std::string{"SDL TTF API failed "} + SDL_GetError(), loc};
        }
    }
#else
    /// Throw if an SDL API call reports failure
    int check_worked(
            int const e,
            std::source_location const &loc = std::source_location::current()) {
        if (e < 0) [[unlikely]] {
            throw felspar::stdexcept::runtime_error{
                    std::string{"SDL TTF API failed "} + SDL_GetError(), loc};
        } else {
            return e;
        }
    }
#endif


}


/// ## `planet::sdl::ttf`


planet::sdl::ttf::ttf(init &) { check_worked(TTF_Init()); }


planet::sdl::ttf::~ttf() { TTF_Quit(); }


/// ## `planet::sdl::font`


planet::sdl::font::font(
        asset_manager const &am,
        char const *const filename,
        std::size_t const pixels,
        SDL_Color const c,
        std::source_location const &loc)
: font_data{am, filename, loc},
  pf{
#if PLANET_SDL3
          TTF_OpenFontIO(font_data.get(), false, static_cast<float>(pixels))
#else
          TTF_OpenFontRW(font_data.get(), false, pixels)
#endif
  },
  colour{c},
  space{measure(" ")},
  em{measure("m")} {
    if (not pf.get()) {
        throw felspar::stdexcept::runtime_error{
#if PLANET_SDL3
                "TTF_OpenFontIO returned nullptr"
#else
                "TTF_OpenFontRW returned nullptr"
#endif
        };
    }
}


planet::affine::extents2d planet::sdl::font::measure(char const *const t) const {
    int w{}, h{};
    check_worked(
#if PLANET_SDL3
            TTF_GetStringSize(pf.get(), t, 0, &w, &h)
#else
            TTF_SizeUTF8(pf.get(), t, &w, &h)
#endif
    );
    return {static_cast<float>(w), static_cast<float>(h)};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text, SDL_Color c, ui::scale const fit) const {
    return {
#if PLANET_SDL3
            TTF_RenderText_Blended(pf.get(), text, 0, c),
#else
            TTF_RenderUTF8_Blended(pf.get(), text, c),
#endif
            fit};
}


planet::sdl::surface planet::sdl::font::render(
        char const *text,
        SDL_Color const c,
        std::uint32_t const width,
        ui::scale const fit) const {
    return {
#if PLANET_SDL3
            TTF_RenderText_Blended_Wrapped(pf.get(), text, 0, c, width),
#else
            TTF_RenderUTF8_Blended_Wrapped(pf.get(), text, c, width),
#endif
            fit};
}
