#include <planet/log.hpp>
#include <planet/sdl/file_dialog.hpp>
#include <planet/sdl/sdl.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <vector>


using namespace std::literals;


namespace {


    /// ## Shared between the coroutine and SDL's completion callback
    struct dialog_state final {
        std::mutex mutex;
        bool ready = false;
        std::optional<std::filesystem::path> path;
        std::optional<std::string> error;
    };


    /// ## SDL's dialog completion callback
    /**
     * SDL invokes this when the dialog closes -- possibly on a different thread
     * than the one that showed it. It must not resume the coroutine directly
     * (`felspar::coro::future` is not thread safe), so it only records the
     * outcome under the lock; `file_dialog`'s loop picks it up on the warden's
     * thread. `SDL_GetError` is read here because the failing thread is the one
     * SDL set the error on.
     */
    void SDLCALL dialog_completed(
            void *const userdata,
            char const *const *const filelist,
            int const) noexcept {
        /**
         * Reclaim the heap `shared_ptr` handed to SDL as `userdata`, releasing
         * the state when this returns independently of the coroutine's own hold
         * (which will have gone already if it was cancelled).
         */
        std::unique_ptr<std::shared_ptr<dialog_state>> const owner{
                static_cast<std::shared_ptr<dialog_state> *>(userdata)};
        auto const state = *owner;

        std::scoped_lock const lock{state->mutex};
        if (filelist == nullptr) { state->error = SDL_GetError(); }
        state->path = planet::sdl::first_selected_path(filelist);
        state->ready = true;
    }


    std::vector<SDL_DialogFileFilter> to_sdl_filters(
            std::span<planet::sdl::file_dialog_filter const> const filters) {
        std::vector<SDL_DialogFileFilter> sdl;
        sdl.reserve(filters.size());
        for (auto &&f : filters) {
            sdl.push_back({f.name.c_str(), f.pattern.c_str()});
        }
        return sdl;
    }


}


std::optional<std::filesystem::path> planet::sdl::first_selected_path(
        char const *const *const filelist) noexcept {
    if (filelist == nullptr or *filelist == nullptr) {
        return {};
    } else {
        return std::filesystem::path{*filelist};
    }
}


felspar::io::warden::task<std::optional<std::filesystem::path>>
        planet::sdl::file_dialog(
                felspar::io::warden &io,
                file_dialog_kind const kind,
                SDL_Window *const parent,
                std::optional<std::filesystem::path> default_location,
                std::span<file_dialog_filter const> const filters) {
    auto const state = std::make_shared<dialog_state>();

    /**
     * SDL requires the filters and default location to stay valid until it
     * invokes the callback. They live on this coroutine frame, which persists
     * across the suspension below.
     */
    auto const sdl_filters = to_sdl_filters(filters);
    auto const *const filters_c =
            sdl_filters.empty() ? nullptr : sdl_filters.data();
    std::optional<std::string> const location = default_location
            ? std::optional{default_location->string()}
            : std::nullopt;
    char const *const location_c = location ? location->c_str() : nullptr;

    /**
     * SDL adopts nothing, so the shared state crosses to the callback as a heap
     * `shared_ptr` copy that `dialog_completed` reclaims.
     */
    auto *const userdata = new std::shared_ptr<dialog_state>{state};
    switch (kind) {
    case file_dialog_kind::open:
        SDL_ShowOpenFileDialog(
                dialog_completed, userdata, parent, filters_c,
                static_cast<int>(sdl_filters.size()), location_c, false);
        break;
    case file_dialog_kind::save:
        SDL_ShowSaveFileDialog(
                dialog_completed, userdata, parent, filters_c,
                static_cast<int>(sdl_filters.size()), location_c);
        break;
    case file_dialog_kind::folder:
        SDL_ShowOpenFolderDialog(
                dialog_completed, userdata, parent, location_c, false);
        break;
    }

    /**
     * Observe the result on the warden's thread. The sleep yields so the event
     * loop keeps pumping SDL while the dialog is open
     */
    while (true) {
        {
            std::scoped_lock const lock{state->mutex};
            if (state->ready) {
                if (state->error) {
                    planet::log::error("SDL file dialog failed", *state->error);
                }
                co_return std::move(state->path);
            }
            /// Do not hold the lock whilst sleeping
        }
        co_await io.sleep(30ms);
    }
}
