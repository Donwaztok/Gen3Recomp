#include "user_data.hpp"

#include <cstdlib>

namespace gen3recomp {

std::filesystem::path user_data_dir() {
    if (const char* xdg = std::getenv("XDG_DATA_HOME"); xdg != nullptr && *xdg != '\0') {
        return std::filesystem::path{xdg} / "gen3recomp";
    }
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        return std::filesystem::path{home} / ".local" / "share" / "gen3recomp";
    }
    return std::filesystem::temp_directory_path() / "gen3recomp";
}

std::filesystem::path recomp_cache_dir(const std::string& rom_sha1) {
    return user_data_dir() / "recomp_cache" / rom_sha1;
}

std::filesystem::path cartridge_save_path(const std::string& rom_sha1) {
    return user_data_dir() / "saves" / (rom_sha1 + ".sav");
}

}
