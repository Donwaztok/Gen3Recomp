#include "catalog.hpp"
#include "sha1.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

TEST_CASE("builtin catalog contains mvp usa titles") {
    const auto catalog = gen3recomp::Catalog::builtin();
    REQUIRE(catalog.find_by_sha1("f28b6ffc97847e94a6c21a63cacf633ee5c8df1e").has_value());
    REQUIRE(catalog.find_by_sha1("3ccbbd45f8553c36463f13b938e833f652b793e4").has_value());
    REQUIRE(catalog.find_by_sha1("f3ae088181bf583e55daf962a92bb46f4f1d07b7").has_value());
    REQUIRE(catalog.find_by_sha1("F3AE088181BF583E55DAF962A92BB46F4F1D07B7")->id == "emerald-usa");
}

TEST_CASE("unknown sha1 is rejected") {
    gen3recomp::Catalog catalog;
    catalog.add({"fixture", "Fixture Game", "USA", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "gen3", "flash1m", false});
    REQUIRE_FALSE(catalog.find_by_sha1("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb").has_value());
    REQUIRE(catalog.find_by_sha1("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")->display_name == "Fixture Game");
}

TEST_CASE("synthetic dump identity matches catalog entry") {
    constexpr std::string_view payload = "synthetic-rom";
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(payload.data());
    const auto digest = sha1_hex({bytes, payload.size()});

    gen3recomp::Catalog catalog;
    catalog.add({"synthetic", "Synthetic", "USA", digest, "gen3", "flash1m", false});
    const auto found = catalog.find_by_sha1(digest);
    REQUIRE(found.has_value());
    REQUIRE(found->id == "synthetic");
}

TEST_CASE("known bios hash is accepted") {
    REQUIRE(gen3recomp::is_known_bios_sha1("300c20df6731a33952ded8c436f7f186d25d3492"));
    REQUIRE(gen3recomp::is_known_bios_sha1("c11531d5261006810cdc954bd4bec0afe3187b35"));
    REQUIRE_FALSE(gen3recomp::is_known_bios_sha1("fd2547724b505f487e6dcb29ec2ecff3af35a841"));
    REQUIRE_FALSE(gen3recomp::is_known_bios_sha1("0000000000000000000000000000000000000000"));
}
