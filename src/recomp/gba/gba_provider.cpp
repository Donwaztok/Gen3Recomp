#include "gba_provider.hpp"

#include "gba_backend.hpp"

#include <cstdlib>
#include <filesystem>
#include <system_error>

namespace gen3recomp {
namespace {

std::filesystem::path default_user_data_dir() {
    if (const char* xdg = std::getenv("XDG_DATA_HOME"); xdg != nullptr && *xdg != '\0') {
        return std::filesystem::path{xdg} / "gen3recomp";
    }
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        return std::filesystem::path{home} / ".local" / "share" / "gen3recomp";
    }
    return std::filesystem::temp_directory_path() / "gen3recomp";
}

}  // namespace

bool GbaRecompProvider::prepare(
    const std::filesystem::path& rom_path,
    const std::filesystem::path& bios_path,
    const GameDefinition& game,
    PreparedSession& out,
    std::string& error) const {
    std::error_code exists_error;
    if (!std::filesystem::is_regular_file(rom_path, exists_error)) {
        error = "ROM path is not a regular file";
        return false;
    }
    if (!std::filesystem::is_regular_file(bios_path, exists_error)) {
        error = "BIOS path is not a regular file";
        return false;
    }
    if (game.sha1.empty() || game.id.empty()) {
        error = "Game Definition is missing id or SHA-1";
        return false;
    }

    const auto cache_dir = default_user_data_dir() / "recomp_cache" / game.sha1;
    std::error_code create_error;
    std::filesystem::create_directories(cache_dir, create_error);
    if (create_error) {
        error = "failed to create recomp cache directory: " + create_error.message();
        return false;
    }

    out.backend = std::make_unique<GbaSessionBackend>(rom_path, bios_path, cache_dir, game);
    return true;
}

}
