#include "audio.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdlib>

TEST_CASE("audio init failure is non fatal") {
    setenv("SDL_AUDIODRIVER", "dummy", 1);
    gen3recomp::AudioDevice audio;
    (void)audio.init();
    const std::array<std::int16_t, 4> silence{};
    REQUIRE_NOTHROW(audio.queue(silence));
    audio.shutdown();
}
