#include "roms_scan.hpp"

#include "file_bytes.hpp"

#include <algorithm>
#include <unordered_set>

namespace gen3recomp {

std::vector<std::filesystem::path> roms_search_roots(
    const std::optional<std::filesystem::path>& exe_dir) {
    std::vector<std::filesystem::path> roots;
    std::error_code error;
    const auto cwd = std::filesystem::current_path(error);
    if (!error) {
        roots.push_back(cwd / "roms");
    }
    if (exe_dir.has_value()) {
        roots.push_back(*exe_dir / "roms");
    }
    return roots;
}

std::vector<DiscoveredRom> scan_catalogued_roms(
    const Catalog& catalog,
    const std::optional<std::filesystem::path>& exe_dir) {
    std::vector<DiscoveredRom> found;
    std::unordered_set<std::string> seen_sha1;
    for (const auto& root : roms_search_roots(exe_dir)) {
        std::error_code error;
        if (!std::filesystem::is_directory(root, error)) {
            continue;
        }
        for (const auto& entry : std::filesystem::directory_iterator(root, error)) {
            if (error || !entry.is_regular_file()) {
                continue;
            }
            const auto ext = entry.path().extension().string();
            if (ext != ".gba" && ext != ".GBA") {
                continue;
            }
            const auto sha1 = sha1_file(entry.path());
            if (!seen_sha1.insert(sha1).second) {
                continue;
            }
            const auto game = catalog.find_by_sha1(sha1);
            if (!game.has_value()) {
                continue;
            }
            found.push_back(DiscoveredRom{entry.path(), sha1, *game});
        }
    }
    std::sort(found.begin(), found.end(), [](const DiscoveredRom& a, const DiscoveredRom& b) {
        return a.game.display_name < b.game.display_name;
    });
    return found;
}

}  // namespace gen3recomp
