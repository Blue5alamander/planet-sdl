#include <planet/sdl/audio.hpp>
#include <planet/sdl/init.hpp>
#include <planet/maths.hpp>

using namespace std::literals;
using namespace planet::audio::literals;


namespace {


    struct spinner {
        spinner(float const frequency)
        : spinner{frequency, planet::audio::linear_gain{}} {}
        template<typename Gain>
        spinner(float const frequency, Gain const g) : gain{g} {
            auto const root =
                    frequency / planet::audio::sample_clock::period::den;
            rotate = {planet::cos(root), planet::sin(root)};
        }
        std::complex<float> rotate, phase{1.0f, 0.0f};
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
        while (frequency < 22'000 and gain > -128_dB) {
            planet::audio::linear_gain const g{gain};
            tones.spinners.emplace_back(
                    frequency, g.load() * master.load() / harmonic);
            planet::log::debug(
                    "Frequency", frequency, "attenuation", gain, "multiplier",
                    tones.spinners.back().gain.load());
            frequency += root;
            gain -= drop_off;
            ++harmonic;
        }
    }
    void harmonics(
            spectrum &tones,
            float const root,
            planet::audio::dB_gain const drop_off,
            planet::audio::dB_gain master) {
        return harmonics(
                tones, root, drop_off,
                static_cast<planet::audio::linear_gain>(master));
    }


}


int main() {
    felspar::io::poll_warden warden;
    warden.run(+[](felspar::io::warden &warden) -> felspar::io::warden::task<void> {
        planet::version const version{"planet-sdl/tone-example", "1.0"};
        planet::sdl::init sdl{warden, version};

        planet::audio::channel master_attenuator{-12_dB},
                mixer_attenuator{0_dB};
        planet::audio::mixer mixer{mixer_attenuator};
        planet::sdl::audio_output output{
                sdl.audio_devices()[0], master_attenuator, mixer};

        spectrum chord;
        harmonics(chord, 440.0f, 6_dB, -6_dB);
        harmonics(chord, 550.0f, 6_dB, -6_dB);
        harmonics(chord, 660.0f, 6_dB, -6_dB);
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
    });

    return 0;
}
