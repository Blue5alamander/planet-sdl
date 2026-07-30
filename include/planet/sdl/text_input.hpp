#pragma once


#include <planet/affine/rectangle2d.hpp>
#include <planet/sdl/sdl.hpp>


namespace planet::sdl {


    /// ## Text input
    /**
     * SDL3 starts with text input **off** on every platform and sends no
     * `SDL_EVENT_TEXT_INPUT` until something switches it on, so a widget that
     * wants the characters a keyboard produced -- rather than the scancodes of
     * the keys that were pressed -- has to ask for them. On mobile that same
     * switch is what raises and dismisses the on-screen keyboard.
     *
     * Text input is per window, hence the window on every call.
     *
     * Areas and offsets are given in the UI's own drawable pixels; converting
     * them into the window points SDL wants is part of what these wrappers are
     * for, so nothing above has to know a display's pixel density.
     */

    /// ### Say where the text being typed is going
    void set_text_input_area(
            SDL_Window *,
            affine::rectangle2d const &area,
            float cursor) noexcept;
    /**
     * Tells the platform where the field is, and through `cursor` -- an offset
     * from the area's left edge -- where the caret within it is, so an
     * on-screen keyboard or an IME candidate list can be placed clear of both.
     *
     * Set the area before starting input: a soft keyboard is positioned as it
     * appears, so by then SDL has to know what it must not cover.
     */

    /// ### Start text input for a window
    void start_text_input(SDL_Window *) noexcept;

    /// ### Stop text input for a window
    void stop_text_input(SDL_Window *) noexcept;


    /// ## A window's text input, as something to call
    /**
     * The three calls above bound to one window and reduced to the only
     * decision there is to make with them: an edit is starting somewhere, or
     * an edit has ended. A widget that has an edit-state seam can bind one of
     * these straight to it and never name SDL itself.
     *
     * The `area` is the widget's own, in whatever coordinates it laid itself
     * out in; `cursor` is the caret's offset from its left edge.
     */
    struct text_entry final {
        SDL_Window *window = nullptr;


        void operator()(
                bool const editing,
                affine::rectangle2d const &area,
                float const cursor) const noexcept {
            if (editing) {
                /**
                 * The area goes first because a soft keyboard is positioned as
                 * it appears, so SDL has to know what it must not cover by the
                 * time input starts.
                 */
                set_text_input_area(window, area, cursor);
                start_text_input(window);
            } else {
                stop_text_input(window);
            }
        }
    };


}
