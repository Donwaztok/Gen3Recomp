#pragma once

#include "catalog.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace gen3recomp {

struct DiscoveredRom {
    std::filesystem::path path;
    std::string sha1;
    GameDefinition game;
};

// Scan roms/ beside CWD and optionally beside the executable parent.
std::vector<std::filesystem::path> roms_search_roots(
    const std::optional<std::filesystem::path>& exe_dir = std::nullopt);

std::vector<DiscoveredRom> scan_catalogued_roms(
    const Catalog& catalog,
    const std::optional<std::filesystem::path>& exe_dir = std::nullopt);

}  // namespace gen3recomp
