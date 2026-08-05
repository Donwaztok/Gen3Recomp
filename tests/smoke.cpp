#include "logging.hpp"
#include "version.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

TEST_CASE("version string is not empty") {
    REQUIRE_FALSE(std::string_view{GEN3RECOMP_VERSION}.empty());
}

TEST_CASE("logging initializes") {
    REQUIRE_NOTHROW(init_logging());
    REQUIRE_NOTHROW(log_error("diagnostic smoke test"));
}
