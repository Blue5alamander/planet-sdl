#pragma once


#include <planet/sdl/sdl.hpp>

#include <felspar/io/warden.hpp>

#include <filesystem>


namespace planet::sdl {


    /// ## Which file dialog to show
    enum class file_dialog_kind {
        open, // Choose an existing file (`SDL_ShowOpenFileDialog`)
        save, // Name a file to write, which need not yet exist
              // (`SDL_ShowSaveFileDialog`)
        folder, // Choose a folder (`SDL_ShowOpenFolderDialog`)
    };


    /// ## A name/pattern pair for filtering the file list
    /**
     * `pattern` is a semicolon separated list of extensions **without** the
     * leading dot, or a single `*` meaning all files. Not every platform
     * honours filters, and those that do may let the user ignore them.
     */
    struct file_dialog_filter final {
        std::string name; // e.g. `"Blue Weave project"`
        std::string pattern; // e.g. `"bw-project"`, or `"*"` for all files
    };


    /// ## Show a native file dialog and await the user's choice
    /**
     * Wraps SDL's asynchronous file dialogs as a coroutine: it shows the
     * dialog and resumes once the user has accepted a path, cancelled, or the
     * dialog has errored. `folder` ignores `filters`.
     *
     * `parent` is the window the dialog is modal for; `nullptr` leaves it
     * parented to nothing, and not every platform honours it either way. The
     * `warden` is the one driving the application's event loop; the coroutine
     * yields to it while the dialog is open so SDL keeps pumping the platform
     * (the Linux portal needs this).
     *
     * Returns the chosen path, or `nullopt` when the user cancelled or the
     * dialog failed (a failure is logged at `error`).
     */
    felspar::io::warden::task<std::optional<std::filesystem::path>> file_dialog(
            felspar::io::warden &,
            file_dialog_kind,
            SDL_Window *parent = nullptr,
            std::optional<std::filesystem::path> default_location = {},
            std::span<file_dialog_filter const> filters = {});


    /// ## Decode an SDL dialog file list into the chosen path
    std::optional<std::filesystem::path>
            first_selected_path(char const *const *filelist) noexcept;
    /**
     * SDL hands the callback a `filelist` that is `nullptr` on error, a pointer
     * to `nullptr` when the user chose nothing, or a null-terminated array of
     * UTF-8 paths otherwise. This returns the first path if one was chosen, and
     * `nullopt` for both the error and the nothing-chosen cases (the caller
     * separates those by testing `filelist` against `nullptr` itself).
     *
     * Exposed so the decode can be exercised without opening a dialog.
     */


}
