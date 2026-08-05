#include "gba_provider.hpp"
#include "cart_artifact.hpp"
#include "user_data.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>

TEST_CASE("cart artifact path is keyed by dump identity and abi tag") {
    const auto path = gen3recomp::cart_artifact_path("dddddddddddddddddddddddddddddddddddddddd");
    REQUIRE(path.filename() == "libcart.so");
    REQUIRE(path.parent_path().filename() == gen3recomp::cart_artifact_abi_tag());
    REQUIRE(path.parent_path().parent_path().filename() == "dddddddddddddddddddddddddddddddddddddddd");
    REQUIRE(path.parent_path().parent_path().parent_path().filename() == "cart_aot");
}

TEST_CASE("missing cart artifact reports heal-only coverage when stub-linked") {
    const auto kind = gen3recomp::detect_cart_coverage("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
    const std::string label = gen3recomp::cart_coverage_label(kind);
    // detect_cart_coverage is compiled inside gen3recomp_recomp. If this binary
    // was linked with a static cart corpus/artifact, coverage is always static
    // regardless of the SHA-1 argument. Otherwise a missing user-data blob is heal-only.
    if (kind == gen3recomp::CartCoverageKind::HealOnly) {
        REQUIRE(label.find("heal") != std::string::npos);
    } else {
        REQUIRE(
            (kind == gen3recomp::CartCoverageKind::LinkedArtifact ||
             kind == gen3recomp::CartCoverageKind::LinkedGeneratedCorpus ||
             kind == gen3recomp::CartCoverageKind::UserDataArtifactPresent));
    }
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
