#include <planet/log.hpp>
#include <planet/sdl/init.hpp>
#include <planet/serialise.hpp>

#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("sdl-config");


    constexpr planet::sdl::window_mode all_modes[] = {
            planet::sdl::window_mode::windowed_fixed_size,
            planet::sdl::window_mode::windowed_resizable,
            planet::sdl::window_mode::full_screen_windowed,
            planet::sdl::window_mode::full_screen_borderless};


    /// Restore `audio::active_buffer_duration` at scope exit
    /**
     * Tests that publish a block size through `configuration::load` mutate the
     * engine-wide global; this puts it back so the suite stays
     * order-independent.
     */
    struct restore_published_block_size {
        planet::audio::sample_clock const saved =
                planet::audio::active_buffer_duration.load(
                        std::memory_order_relaxed);
        ~restore_published_block_size() {
            planet::audio::active_buffer_duration.store(
                    saved, std::memory_order_relaxed);
        }
    };


    /// Every `window_mode` survives a trip through its string form
    auto const strings =
            suite.test("window_mode_strings", [](auto check, auto &) {
                for (auto const m : all_modes) {
                    check(planet::sdl::window_mode_from_string(
                                  planet::sdl::to_string(m))
                          == m)
                            == true;
                }
            });


    /// The legacy `"windowed"` spelling still loads as the fixed-size mode
    auto const legacy =
            suite.test("window_mode_legacy_windowed", [](auto check, auto &) {
                check(planet::sdl::window_mode_from_string("windowed")
                      == planet::sdl::window_mode::windowed_fixed_size)
                        == true;
            });


    /**
     * A single `configuration` is used for the whole test: its
     * `telemetry::counter` members register themselves in a global index by
     * name, so a second live instance in the same process would throw. This
     * mirrors the game, which owns exactly one `configuration`.
     */
    auto const window_config =
            suite.test("window_config", [](auto check, auto &) {
                planet::sdl::configuration c;

                /// ### Each mode + geometry survives a save/load round-trip
                for (auto const m : all_modes) {
                    planet::affine::extents2d const extents{800, 600};
                    planet::affine::point2d const position{10, 20};
                    c.window_display_mode = m;
                    c.window_extents = extents;
                    c.window_position = position;

                    planet::serialise::save_buffer ab;
                    save(ab, c);
                    auto const bytes = ab.complete();

                    // Clobber the fields so a successful reload is meaningful.
                    c.window_display_mode =
                            planet::sdl::window_mode::windowed_fixed_size;
                    c.window_extents = {0, 0};
                    c.window_position = {};

                    planet::serialise::load_buffer lb{bytes.cmemory()};
                    load(lb, c);

                    check(c.window_display_mode == m) == true;
                    check(c.window_extents == extents) == true;
                    check(c.window_position == std::optional{position}) == true;
                }

                /// ### A pre-window configuration still loads, without touching
                /// the window fields (the append-only guard)
                c.window_display_mode =
                        planet::sdl::window_mode::full_screen_borderless;
                c.window_extents = {640, 480};
                c.window_position = planet::affine::point2d{5, 5};
                c.auto_remove_log_files = false;

                /**
                 * Serialise only the fields that existed before the window
                 * configuration was appended, exactly as an older build would
                 * have written them.
                 */
                planet::serialise::save_buffer ab;
                ab.save_box(
                        c.box, c.log_level, c.auto_remove_log_files,
                        c.master_volume, c.music_volume, c.sfx_volume,
                        c.audio_device_name, c.save_logs_to_file,
                        c.upload_performance_data,
                        c.audio_latency_injected_block_count);
                auto const bytes = ab.complete();

                // Flip the earlier field to prove it reloads; leave the window
                // fields to prove an old buffer does not overwrite them.
                c.auto_remove_log_files = true;
                planet::serialise::load_buffer lb{bytes.cmemory()};
                load(lb, c);

                check(c.auto_remove_log_files) == false;
                check(c.window_display_mode
                      == planet::sdl::window_mode::full_screen_borderless)
                        == true;
                check(c.window_extents == planet::affine::extents2d{640, 480})
                        == true;
                check(c.window_position
                      == std::optional{planet::affine::point2d{5, 5}})
                        == true;
            });


    auto const audio_config = suite.test(
            "audio",
            [](auto check, auto &) {
                /// ### `default_buffer_duration` survives a save/load round-trip
                planet::sdl::configuration c{planet::log::level::critical};
                c.default_buffer_duration = planet::audio::sample_clock{1024};
                planet::serialise::save_buffer ab;
                save(ab, c);
                auto const bytes = ab.complete();

                // Clobber so a successful reload is meaningful.
                c.default_buffer_duration = planet::audio::sample_clock{512};

                planet::serialise::load_buffer lb{bytes.cmemory()};
                load(lb, c);

                check(c.default_buffer_duration
                      == planet::audio::sample_clock{1024})
                        == true;
            },
            [](auto check) {
                /// ### A pre-`default_buffer_duration` configuration still loads
                /**
                 * Serialise only the fields that existed before
                 * `default_buffer_duration` was appended, exactly as an older
                 * build would have written them. The field is absent from the
                 * buffer, so the load guard returns before reaching it and it
                 * keeps its 512 default.
                 */
                planet::sdl::configuration c{planet::log::level::critical};
                c.default_buffer_duration = planet::audio::sample_clock{512};
                c.auto_remove_log_files = false;
                planet::serialise::save_buffer ab;
                ab.save_box(
                        c.box, c.log_level, c.auto_remove_log_files,
                        c.master_volume, c.music_volume, c.sfx_volume,
                        c.audio_device_name, c.save_logs_to_file,
                        c.upload_performance_data,
                        c.audio_latency_injected_block_count,
                        c.window_display_mode, c.window_extents,
                        c.window_position);
                auto const bytes = ab.complete();

                /// Flip an earlier field to prove the reload still ran.
                c.auto_remove_log_files = true;
                planet::serialise::load_buffer lb{bytes.cmemory()};
                load(lb, c);

                check(c.auto_remove_log_files) == false;
                check(c.default_buffer_duration
                      == planet::audio::sample_clock{512})
                        == true;
            },
            [](auto check) {
                /// ### A valid non-512 block size is published to the engine on
                /// load
                /**
                 * `load` writes the configured block size into the published
                 * `audio::active_buffer_duration` so the generators below
                 * `planet-sdl` render at it. The guard restores the published
                 * global afterwards so this mutation does not leak into other
                 * suites.
                 */
                planet::sdl::configuration c{planet::log::level::critical};
                restore_published_block_size const restore;

                c.default_buffer_duration = planet::audio::sample_clock{1024};
                planet::serialise::save_buffer ab;
                save(ab, c);
                auto const bytes = ab.complete();

                // Clobber the member and reset the published global so a
                // successful reload is meaningful.
                c.default_buffer_duration =
                        planet::audio::initial_buffer_duration;
                planet::audio::active_buffer_duration.store(
                        planet::audio::initial_buffer_duration);

                planet::serialise::load_buffer lb{bytes.cmemory()};
                load(lb, c);

                check(planet::audio::buffer_samples() == std::size_t{1024})
                        == true;
            },
            [](auto check) {
                /// ### An out-of-range block size is clamped, not forwarded as-is
                /**
                 * A zero or above-cap block size would build a driver that
                 * cannot work, so `load` clamps it into `(0,
                 * audio::max_buffer_duration]` and logs an error. Both the
                 * member and the published `buffer_samples()` therefore stay
                 * usable, which is what `reconnect` reads to build the driver.
                 */
                planet::sdl::configuration c{planet::log::level::critical};
                restore_published_block_size const restore;
                /**
                 * We don't want to show the error message in the test output.
                 */

                /// Zero clamps up to the initial working size.
                c.default_buffer_duration = planet::audio::sample_clock{0};
                {
                    planet::serialise::save_buffer ab;
                    save(ab, c);
                    auto const bytes = ab.complete();
                    planet::serialise::load_buffer lb{bytes.cmemory()};
                    load(lb, c);
                }
                check(c.default_buffer_duration
                      == planet::audio::initial_buffer_duration)
                        == true;
                check(planet::audio::buffer_samples()
                      == planet::audio::initial_buffer_duration.count())
                        == true;

                /// Above the cap clamps down to it.
                c.default_buffer_duration = planet::audio::sample_clock{
                        planet::audio::max_buffer_duration.count() + 1};
                {
                    planet::serialise::save_buffer ab;
                    save(ab, c);
                    auto const bytes = ab.complete();
                    planet::serialise::load_buffer lb{bytes.cmemory()};
                    load(lb, c);
                }
                check(c.default_buffer_duration
                      == planet::audio::max_buffer_duration)
                        == true;
                check(planet::audio::buffer_samples()
                      == planet::audio::max_buffer_samples)
                        == true;
            });


}
