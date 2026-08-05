#include "sha1.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

TEST_CASE("sha1 empty string vector") {
    const std::vector<std::uint8_t> empty;
    REQUIRE(sha1_bytes_hex(empty) == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST_CASE("sha1 abc vector") {
    constexpr std::string_view text = "abc";
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(text.data());
    REQUIRE(sha1_hex({bytes, text.size()}) == "a9993e364706816aba3e25717850c26c9cd0d89d");
}
