#include "cart_artifact.hpp"
#include "cover_art.hpp"
#include "mods.hpp"
#include "roms_scan.hpp"
#include "user_data.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

std::filesystem::path unique_temp_dir(const std::string& name) {
    const auto path = std::filesystem::temp_directory_path() / name;
    std::filesystem::remove_all(path);
    std::filesystem::create_directories(path);
    return path;
}

}  // namespace

TEST_CASE("cart_artifact_ready is false for missing dump") {
    REQUIRE_FALSE(gen3recomp::cart_artifact_ready("ffffffffffffffffffffffffffffffffffffffff"));
}

TEST_CASE("cover urls are curated for MVP titles") {
    const auto emerald = gen3recomp::cover_urls_for("emerald-usa");
    REQUIRE_FALSE(emerald.primary.empty());
    REQUIRE(emerald.primary.find("http") == 0);
    REQUIRE(gen3recomp::cover_urls_for("not-a-game").primary.empty());
}

TEST_CASE("missing cover yields no cache path") {
    const auto temp = unique_temp_dir("gen3recomp-cover-miss");
#if defined(_WIN32)
    _putenv_s("XDG_DATA_HOME", temp.string().c_str());
#else
    setenv("XDG_DATA_HOME", temp.string().c_str(), 1);
#endif
    REQUIRE_FALSE(gen3recomp::find_cached_cover("emerald-usa").has_value());
    std::filesystem::remove_all(temp);
}

TEST_CASE("cover cache hit prefers existing file without network") {
    const auto temp = unique_temp_dir("gen3recomp-cover-hit");
#if defined(_WIN32)
    _putenv_s("XDG_DATA_HOME", temp.string().c_str());
#else
    setenv("XDG_DATA_HOME", temp.string().c_str(), 1);
#endif
    const auto cached = gen3recomp::cover_cache_path("emerald-usa");
    std::filesystem::create_directories(cached.parent_path());
    {
        std::ofstream out{cached, std::ios::binary};
        out << "fake-cover-bytes-not-an-image-but-present";
    }
    const auto found = gen3recomp::find_cached_cover("emerald-usa");
    REQUIRE(found.has_value());
    REQUIRE(*found == cached);
    std::filesystem::remove_all(temp);
}

TEST_CASE("mod enablement persists under user data") {
    const auto temp = unique_temp_dir("gen3recomp-mods-test");
#if defined(_WIN32)
    _putenv_s("XDG_DATA_HOME", temp.string().c_str());
#else
    setenv("XDG_DATA_HOME", temp.string().c_str(), 1);
#endif

    const auto mod_root = gen3recomp::user_data_dir() / "mods" / "sample_mod";
    std::filesystem::create_directories(mod_root);
    {
        std::ofstream out{mod_root / "mod.toml"};
        out << "name = \"Sample\"\nversion = \"1.0\"\n";
    }

    std::string error;
    REQUIRE(gen3recomp::set_mod_enabled("sample_mod", true, error));
    auto mods = gen3recomp::discover_mods();
    REQUIRE(mods.size() == 1);
    REQUIRE(mods[0].enabled);
    REQUIRE(gen3recomp::enabled_mod_ids().size() == 1);

    REQUIRE(gen3recomp::set_mod_enabled("sample_mod", false, error));
    mods = gen3recomp::discover_mods();
    REQUIRE_FALSE(mods[0].enabled);

    std::filesystem::remove_all(temp);
}

TEST_CASE("mod with embedded gba is not enableable") {
    const auto temp = unique_temp_dir("gen3recomp-mods-forbid");
#if defined(_WIN32)
    _putenv_s("XDG_DATA_HOME", temp.string().c_str());
#else
    setenv("XDG_DATA_HOME", temp.string().c_str(), 1);
#endif

    const auto mod_root = gen3recomp::user_data_dir() / "mods" / "bad_mod";
    std::filesystem::create_directories(mod_root);
    {
        std::ofstream out{mod_root / "mod.toml"};
        out << "name = \"Bad\"\nversion = \"1.0\"\n";
    }
    {
        std::ofstream out{mod_root / "secret.gba", std::ios::binary};
        out << "not-a-rom";
    }

    auto mods = gen3recomp::discover_mods();
    REQUIRE(mods.size() == 1);
    REQUIRE_FALSE(mods[0].valid);
    std::string error;
    REQUIRE_FALSE(gen3recomp::set_mod_enabled("bad_mod", true, error));
    REQUIRE(error.find("ROM") != std::string::npos);

    std::filesystem::remove_all(temp);
}

TEST_CASE("roms scan roots include cwd roms folder") {
    const auto roots = gen3recomp::roms_search_roots(std::nullopt);
    REQUIRE_FALSE(roots.empty());
    REQUIRE(roots.front().filename() == "roms");
}
