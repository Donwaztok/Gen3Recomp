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

std::string cart_artifact_abi_tag() {
    // Keep in sync with scripts/build_cart_artifact.sh. Bump when the generated
    // C calling convention or overlay ABI changes.
    return "abi3-linux-x64";
}

std::filesystem::path cart_artifact_dir(const std::string& rom_sha1) {
    return user_data_dir() / "cart_aot" / rom_sha1 / cart_artifact_abi_tag();
}

std::filesystem::path cart_artifact_path(const std::string& rom_sha1) {
    return cart_artifact_dir(rom_sha1) / "libcart.so";
}

}
