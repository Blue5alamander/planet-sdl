#include <planet/sdl/ttf.hpp>
#include <planet/ui/scale.hpp>

#include <felspar/exceptions/runtime_error.hpp>

#include <source_location>
#include <string>


namespace {


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


    /// Throw if an SDL TTF render call produced no surface
    /**
     * SDL3's text rendering calls return a null `SDL_Surface *` on failure,
     * including for an empty string (which has zero width). Turn that into an
     * exception here so it never reaches `planet::sdl::surface`, whose
     * null-tolerant handle would otherwise carry the failure far from its
     * cause before crashing.
     */
    SDL_Surface *checked_surface(
            SDL_Surface *const s,
            std::source_location const &loc = std::source_location::current()) {
        if (not s) [[unlikely]] {
            throw felspar::stdexcept::runtime_error{
                    std::string{"SDL TTF rendering produced no surface\n"}
                            + SDL_GetError(),
                    loc};
        } else {
            return s;
        }
    }


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
  pf{TTF_OpenFontIO(font_data.get(), false, static_cast<float>(pixels))},
  colour{c},
  space{measure(" ")},
  em{measure("m")} {
    if (not pf.get()) [[unlikely]] {
        throw felspar::stdexcept::runtime_error{
                "TTF_OpenFontIO returned nullptr"};
    }
}


planet::affine::extents2d
        planet::sdl::font::measure(std::string_view const t) const {
    int w{}, h{};
    check_worked(TTF_GetStringSize(pf.get(), t.data(), t.size(), &w, &h));
    return {static_cast<float>(w), static_cast<float>(h)};
}


planet::sdl::surface planet::sdl::font::render(
        std::string_view const text, SDL_Color c, ui::scale const fit) const {
    return {checked_surface(TTF_RenderText_Blended(
                    pf.get(), text.data(), text.size(), c)),
            fit};
}


planet::sdl::surface planet::sdl::font::render(
        std::string_view const text,
        SDL_Color const c,
        std::uint32_t const width,
        ui::scale const fit) const {
    return {checked_surface(TTF_RenderText_Blended_Wrapped(
                    pf.get(), text.data(), text.size(), c, width)),
            fit};
}
