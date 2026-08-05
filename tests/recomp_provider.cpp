#include "catalog.hpp"
#include "file_bytes.hpp"
#include "gba_provider.hpp"
#include "null_backend.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

namespace {

std::filesystem::path write_temp(const std::string& name, std::string_view contents) {
    const auto path = std::filesystem::temp_directory_path() / name;
    std::ofstream out{path, std::ios::binary};
    out << contents;
    return path;
}

}  // namespace

TEST_CASE("fake null backend still satisfies the session contract") {
    gen3recomp::NullBackend backend;
    REQUIRE(backend.start());
    const auto frame = backend.step();
    REQUIRE(frame.width == 240);
    REQUIRE(frame.height == 160);
    backend.stop();
}

TEST_CASE("gba provider prepare succeeds for existing files") {
    const auto rom = write_temp("gen3recomp-provider.rom", "rom");
    const auto bios = write_temp("gen3recomp-provider.bios", "bios");
    gen3recomp::GameDefinition game{
        "fixture",
        "Fixture",
        "USA",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "gen3",
        "flash1m",
        false,
    };

    gen3recomp::GbaRecompProvider provider;
    gen3recomp::PreparedSession prepared;
    std::string error;
    REQUIRE(provider.prepare(rom, bios, game, prepared, error));
    REQUIRE(prepared.backend != nullptr);
    REQUIRE(prepared.backend->owns_host_loop());
    std::filesystem::remove(rom);
    std::filesystem::remove(bios);
}

TEST_CASE("gba provider prepare fails for missing rom") {
    gen3recomp::GbaRecompProvider provider;
    gen3recomp::PreparedSession prepared;
    std::string error;
    gen3recomp::GameDefinition game{"fixture", "Fixture", "USA", "aa", "gen3", "flash1m", false};
    REQUIRE_FALSE(provider.prepare("/no/rom", "/no/bios", game, prepared, error));
    REQUIRE_FALSE(error.empty());
}

TEST_CASE("optional local dump prepare is env gated") {
    const char* rom = std::getenv("GEN3RECOMP_TEST_ROM");
    const char* bios = std::getenv("GEN3RECOMP_TEST_BIOS");
    if (rom == nullptr || bios == nullptr || *rom == '\0' || *bios == '\0') {
        SKIP("GEN3RECOMP_TEST_ROM / GEN3RECOMP_TEST_BIOS not set");
    }

    gen3recomp::GameDefinition game{
        "env-gated",
        "Env",
        "USA",
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "gen3",
        "flash1m",
        true,
    };
    gen3recomp::GbaRecompProvider provider;
    gen3recomp::PreparedSession prepared;
    std::string error;
    REQUIRE(provider.prepare(rom, bios, game, prepared, error));
    REQUIRE(prepared.backend != nullptr);
}

TEST_CASE("optional local dump headless boot is env gated") {
    SKIP("upstream headless --frames currently aborts; use the windowed boot path");
}
