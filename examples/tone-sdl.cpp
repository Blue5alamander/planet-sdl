#include <planet/sdl/audio.hpp>
#include <planet/sdl/init.hpp>

using namespace std::literals;
using namespace planet::audio::literals;


int main() {
    felspar::io::poll_warden warden;
    warden.run(+[](felspar::io::warden &warden) -> felspar::io::warden::task<void> {
        planet::version const version{"planet-sdl/tone-example", "1.0"};
        planet::sdl::init sdl{warden, version};

        planet::audio::channel master_attenuator{-6_dB}, tones_attenuator{0_dB};
        planet::audio::mixer mixer{tones_attenuator};
        planet::sdl::audio_output output{sdl.audio_devices()[0], master_attenuator, mixer};

        mixer.add_track(planet::audio::stereobuffer(
            planet::audio::monobuffer<
            planet::audio::sample_clock>(
                planet::audio::oscillator(
                    440.0f
                    / planet::audio::sample_clock::
                    period::den))));

        mixer.add_track(planet::audio::gain(
            -6_dB, planet::audio::stereobuffer(
            planet::audio::monobuffer<
            planet::audio::sample_clock>(
                planet::audio::oscillator(
                    660.0f
                    / planet::audio::sample_clock::
                    period::den)))));

        co_await warden.sleep(10s);

        for (planet::audio::dB_gain gain{0_dB}; gain > -128_dB; gain -= 1_dB) {
            tones_attenuator.update(gain);
            co_await warden.sleep(10ms);
        }
    });

    return 0;
}
