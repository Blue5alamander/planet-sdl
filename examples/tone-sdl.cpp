/**
 * Build the example and run it passing in pairs of wave type and frequencies.
 * The available wave types are:
 *
 * * `h 220` -- 220Hz harmonic falling off at 6dB per harmonic
 * * `saw 220` -- 220Hz sawtooth wave
 * * `sin 220 -6` -- 220Hz sine wave at -6dB
 * * `sq 220` -- 220Hz square wave
 * * `tri 220` -- 220Hz triangular wave
 */
#include <planet/sdl/audio.hpp>
#include <planet/sdl/init.hpp>
#include <planet/maths.hpp>

#include <felspar/exceptions/runtime_error.hpp>

using namespace std::literals;
using namespace planet::audio::literals;


namespace {


    template<typename T>
    auto head(std::vector<T> &v) {
        if (v.empty()) {
            throw felspar::stdexcept::runtime_error{"The vector is empty"};
        }
        auto h = v.front();
        v.erase(v.begin());
        return h;
    }


    struct spinner {
        spinner(float const frequency,
                std::complex<float> const p = {1.0f, 0.0f})
        : spinner{frequency, planet::audio::linear_gain{}, p} {}
        template<typename Gain>
        spinner(float const f,
                Gain const g,
                std::complex<float> const p = {1.0f, 0.0f})
        : frequency{f}, phase{p}, gain{g} {
            auto const root =
                    frequency / planet::audio::sample_clock::period::den;
            rotate = {planet::cos(root), planet::sin(root)};
        }
        float frequency;
        std::complex<float> rotate, phase;
        planet::audio::linear_gain gain;

        float step() {
            auto const s = phase.imag();
            phase *= rotate;
            return gain.load() * s;
        }
    };

    struct spectrum {
        std::vector<spinner> spinners;
        std::array<float, planet::audio::default_buffer_samples> buffer = {};

        felspar::coro::generator<std::span<float>> signal() {
            while (true) {
                for (auto &s : buffer) {
                    s = {};
                    for (auto &osc : spinners) { s += osc.step(); }
                }
                co_yield buffer;
            }
        }
    };

    void harmonics(
            spectrum &tones,
            float const root,
            planet::audio::dB_gain const drop_off,
            planet::audio::linear_gain master = {}) {
        std::size_t harmonic{1};
        auto frequency = root;
        planet::audio::dB_gain gain;
        while (frequency < 24'000 and gain > -128_dB) {
            planet::audio::linear_gain const g{gain};
            tones.spinners.emplace_back(
                    frequency, g.load() * master.load() / harmonic);
            planet::log::debug(
                    "Harmonic ", harmonic, "frequency", frequency,
                    "attenuation", gain, "multiplier",
                    tones.spinners.back().gain.load());
            frequency += root;
            gain -= drop_off;
            ++harmonic;
        }
    }
    auto harmonics(
            spectrum &tones,
            float const root,
            planet::audio::dB_gain const drop_off,
            planet::audio::dB_gain const master) {
        return harmonics(
                tones, root, drop_off,
                static_cast<planet::audio::linear_gain>(master));
    }


    void sawtooth(
            spectrum &tones,
            float const root,
            planet::audio::linear_gain const master = {}) {
        std::size_t harmonic{1};
        auto frequency = root;
        while (frequency < 24'000) {
            tones.spinners.emplace_back(frequency, master.load() / harmonic);
            planet::log::debug(
                    "Sawtooth harmonic", harmonic, "frequency", frequency);
            frequency += root;
            ++harmonic;
        }
    }
    void sawtooth(
            spectrum &tones,
            float const root,
            planet::audio::dB_gain const master) {
        return sawtooth(
                tones, root, static_cast<planet::audio::linear_gain>(master));
    }

    void
            square(spectrum &tones,
                   float const root,
                   planet::audio::linear_gain const master = {}) {
        std::size_t harmonic{1};
        auto frequency = root;
        while (frequency < 24'000) {
            tones.spinners.emplace_back(
                    frequency, master.load() / (2 * harmonic - 1));
            planet::log::debug(
                    "Square harmonic", harmonic, "frequency", frequency);
            frequency += 2 * root;
            ++harmonic;
        }
    }
    void
            square(spectrum &tones,
                   float const root,
                   planet::audio::dB_gain const master) {
        return square(
                tones, root, static_cast<planet::audio::linear_gain>(master));
    }

    void triangle(
            spectrum &tones,
            float const root,
            planet::audio::linear_gain const master = {}) {
        std::size_t harmonic{1};
        float phase = 1;
        auto frequency = root;
        while (frequency < 24'000) {
            auto const gain = master.load() / (4 * harmonic * harmonic);
            tones.spinners.emplace_back(
                    frequency, gain, std::complex{phase, 0.0f});
            planet::log::debug(
                    "Triangle harmonic", harmonic, "frequency", frequency,
                    "phase", phase, "gain", gain);
            phase *= -1;
            frequency += 2 * root;
            ++harmonic;
        }
    }
    void triangle(
            spectrum &tones,
            float const root,
            planet::audio::dB_gain const master) {
        return triangle(
                tones, root, static_cast<planet::audio::linear_gain>(master));
    }


    felspar::io::warden::task<void>
            play(felspar::io::warden &warden, std::vector<std::string> args) {
        planet::version const version{"planet-sdl/tone-example", "1.0"};
        planet::sdl::init sdl{warden, version};

        planet::audio::channel master_attenuator{-12_dB},
                mixer_attenuator{0_dB};
        planet::audio::mixer mixer{mixer_attenuator};
        planet::sdl::audio_output output{
                sdl.audio_devices()[0], master_attenuator, mixer};

        spectrum chord;
        while (not args.empty()) {
            auto type = head(args);
            if (type == "h") {
                harmonics(chord, std::stof(head(args)), 6_dB, -6_dB);
            } else if (type == "saw") {
                sawtooth(chord, std::stof(head(args)), -6_dB);
            } else if (type == "sin") {
                auto const freq = std::stof(head(args));
                planet::audio::dB_gain const gain{std::stof(head(args))};
                chord.spinners.emplace_back(freq, gain);
                planet::log::debug("Sine wave frequency", freq, "gain", gain);
            } else if (type == "sq") {
                square(chord, std::stof(head(args)), -6_dB);
            } else if (type == "tri") {
                triangle(chord, std::stof(head(args)), -6_dB);
            }
        }
        mixer.add_track(
                planet::audio::stereobuffer(
                        planet::audio::monobuffer<planet::audio::sample_clock>(
                                chord.signal())));

        planet::log::info("Playing", chord.spinners.size(), "oscillators");

        co_await warden.sleep(10s);

        planet::log::debug("Starting final fade out");
        for (planet::audio::dB_gain gain{0_dB}; gain > -128_dB; gain -= 3_dB) {
            mixer_attenuator.update(gain);
            co_await warden.sleep(100ms);
        }
    }
}


int main(int const argc, char const *const argv[]) {
    felspar::io::poll_warden warden;
    std::vector<std::string> args;
    for (int a{1}; a < argc; ++a) { args.push_back(argv[a]); }
    warden.run(play, std::move(args));
    return 0;
}
