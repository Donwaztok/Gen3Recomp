#include "cli.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

struct Argv {
    std::vector<std::string> storage;
    std::vector<char*> pointers;

    explicit Argv(std::vector<std::string> args) : storage(std::move(args)) {
        pointers.reserve(storage.size());
        for (auto& arg : storage) {
            pointers.push_back(arg.data());
        }
    }

    int argc() const { return static_cast<int>(pointers.size()); }
    char** argv() { return pointers.data(); }
};

std::filesystem::path make_temp_file(const std::string& name) {
    const auto path = std::filesystem::temp_directory_path() / name;
    std::ofstream out{path, std::ios::binary};
    out << "fixture";
    out.close();
    return path;
}

}  // namespace

TEST_CASE("help prints usage and succeeds") {
    Argv args{{"gen3recomp", "--help"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::Ok);
    REQUIRE(result.request.show_help);
    REQUIRE(result.message.find("--rom") != std::string::npos);
    REQUIRE(result.message.find("--bios") != std::string::npos);
    REQUIRE(result.message.find("--version") != std::string::npos);
    REQUIRE(result.message.find("--prepare") != std::string::npos);
    REQUIRE(result.message.find("Exit codes") != std::string::npos);
}

TEST_CASE("version request succeeds") {
    Argv args{{"gen3recomp", "--version"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::Ok);
    REQUIRE(result.request.show_version);
}

TEST_CASE("unknown option is a usage error") {
    Argv args{{"gen3recomp", "--nope"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::UsageError);
    REQUIRE(result.message.find("unknown option") != std::string::npos);
}

TEST_CASE("missing rom option is a usage error") {
    Argv args{{"gen3recomp"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::UsageError);
    REQUIRE(result.message.find("--rom is required") != std::string::npos);
}

TEST_CASE("missing rom file is an input error") {
    Argv args{{"gen3recomp", "--rom", "/no/such/rom.gba"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::InputError);
    REQUIRE(result.message.find("ROM file not found") != std::string::npos);
}

TEST_CASE("existing rom path is accepted") {
    const auto rom = make_temp_file("gen3recomp-launcher-rom.gba");
    Argv args{{"gen3recomp", "--rom", rom.string()}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::Ok);
    REQUIRE(result.request.rom_path == rom.string());
    std::filesystem::remove(rom);
}

TEST_CASE("prepare defaults to 3600 frames") {
    const auto rom = make_temp_file("gen3recomp-launcher-prepare.gba");
    Argv args{{"gen3recomp", "--rom", rom.string(), "--prepare"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::Ok);
    REQUIRE(result.request.prepare_cache);
    REQUIRE(result.request.prepare_frames == 3600);
    std::filesystem::remove(rom);
}

TEST_CASE("prepare accepts an explicit frame count") {
    const auto rom = make_temp_file("gen3recomp-launcher-prepare-n.gba");
    Argv args{{"gen3recomp", "--rom", rom.string(), "--prepare", "1200"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::Ok);
    REQUIRE(result.request.prepare_cache);
    REQUIRE(result.request.prepare_frames == 1200);
    std::filesystem::remove(rom);
}

TEST_CASE("prepare rejects a non-positive frame count") {
    const auto rom = make_temp_file("gen3recomp-launcher-prepare-bad.gba");
    Argv args{{"gen3recomp", "--rom", rom.string(), "--prepare", "0"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::UsageError);
    std::filesystem::remove(rom);
}

TEST_CASE("missing bios file is an input error") {
    const auto rom = make_temp_file("gen3recomp-launcher-rom-bios.gba");
    Argv args{{"gen3recomp", "--rom", rom.string(), "--bios", "/no/such/bios.bin"}};
    const auto result = gen3recomp::parse_args(args.argc(), args.argv());
    REQUIRE(result.code == gen3recomp::ExitCode::InputError);
    REQUIRE(result.message.find("BIOS file not found") != std::string::npos);
    std::filesystem::remove(rom);
}
