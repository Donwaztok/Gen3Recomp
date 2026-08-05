#include "null_backend.hpp"
#include "platform.hpp"
#include "session.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>

TEST_CASE("null session starts ticks and stops") {
    setenv("SDL_VIDEO_DRIVER", "dummy", 1);
    gen3recomp::Platform platform;
    if (!platform.init("gen3recomp-session-test", 240, 160)) {
        WARN("dummy SDL video driver unavailable; skipping session tick assertion");
        platform.shutdown();
        return;
    }

    gen3recomp::NullBackend backend;
    gen3recomp::Session session;
    REQUIRE(session.start(backend));
    REQUIRE(session.tick(platform, backend));
    REQUIRE(session.tick(platform, backend));
    session.stop(backend);
    platform.shutdown();
}
