#include "video.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>

TEST_CASE("integer window scale is logical size times scale") {
    REQUIRE(gen3recomp::window_size_for_scale(240, 4) == 960);
    REQUIRE(gen3recomp::window_size_for_scale(160, 4) == 640);
    REQUIRE(gen3recomp::window_size_for_scale(240, 0) == 0);
}

TEST_CASE("placeholder frame fills every pixel") {
    std::vector<std::uint32_t> pixels(static_cast<std::size_t>(gen3recomp::kGbaWidth * gen3recomp::kGbaHeight), 0);
    gen3recomp::fill_placeholder_frame(pixels, gen3recomp::kGbaWidth, gen3recomp::kGbaHeight);
    REQUIRE(pixels.front() != 0);
    REQUIRE(pixels.back() != 0);
}
