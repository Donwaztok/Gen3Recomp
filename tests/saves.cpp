#include "gba_provider.hpp"
#include "user_data.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>

TEST_CASE("cartridge saves are keyed by dump identity") {
    const auto ruby = gen3recomp::cartridge_save_path("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    const auto emerald = gen3recomp::cartridge_save_path("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    REQUIRE(ruby != emerald);
    REQUIRE(ruby.filename().string().find("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") != std::string::npos);
    REQUIRE(emerald.parent_path().filename() == "saves");
}

TEST_CASE("provider prepare creates a per-identity save path") {
    const auto rom = std::filesystem::temp_directory_path() / "gen3recomp-save.rom";
    const auto bios = std::filesystem::temp_directory_path() / "gen3recomp-save.bios";
    {
        std::ofstream{rom, std::ios::binary} << "rom";
        std::ofstream{bios, std::ios::binary} << "bios";
    }

    gen3recomp::GameDefinition game{
        "fixture-save",
        "Fixture",
        "USA",
        "cccccccccccccccccccccccccccccccccccccccc",
        "gen3",
        "flash1m",
        true,
    };
    gen3recomp::GbaRecompProvider provider;
    gen3recomp::PreparedSession prepared;
    std::string error;
    REQUIRE(provider.prepare(rom, bios, game, prepared, error));
    REQUIRE(std::filesystem::exists(gen3recomp::cartridge_save_path(game.sha1).parent_path()));
    std::filesystem::remove(rom);
    std::filesystem::remove(bios);
}
