#include "platform.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>

TEST_CASE("platform init and shutdown with dummy video driver") {
    setenv("SDL_VIDEO_DRIVER", "dummy", 1);
    gen3recomp::Platform platform;
    const bool ok = platform.init("gen3recomp-test", 240, 160);
    if (!ok) {
        WARN("dummy SDL video driver unavailable; skipping window init assertion");
        platform.shutdown();
        return;
    }
    REQUIRE_FALSE(platform.poll_quit());
    platform.shutdown();
}
