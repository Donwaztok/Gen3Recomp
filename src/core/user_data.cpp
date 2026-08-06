#include "user_data.hpp"

#include <cstdlib>

namespace gen3recomp {

std::filesystem::path user_data_dir() {
    if (const char* xdg = std::getenv("XDG_DATA_HOME"); xdg != nullptr && *xdg != '\0') {
        return std::filesystem::path{xdg} / "gen3recomp";
    }
#if defined(_WIN32)
    if (const char* appdata = std::getenv("APPDATA"); appdata != nullptr && *appdata != '\0') {
        return std::filesystem::path{appdata} / "gen3recomp";
    }
    if (const char* profile = std::getenv("USERPROFILE"); profile != nullptr && *profile != '\0') {
        return std::filesystem::path{profile} / "AppData" / "Roaming" / "gen3recomp";
    }
#elif defined(__APPLE__)
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        return std::filesystem::path{home} / "Library" / "Application Support" / "gen3recomp";
    }
#else
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        return std::filesystem::path{home} / ".local" / "share" / "gen3recomp";
    }
#endif
    return std::filesystem::temp_directory_path() / "gen3recomp";
}

std::filesystem::path recomp_cache_dir(const std::string& rom_sha1) {
    return user_data_dir() / "recomp_cache" / rom_sha1;
}

std::filesystem::path cartridge_save_path(const std::string& rom_sha1) {
    return user_data_dir() / "saves" / (rom_sha1 + ".sav");
}

std::string cart_artifact_abi_tag() {
#if defined(_WIN32)
    return "abi3-windows-x64";
#elif defined(__APPLE__)
#if defined(__aarch64__)
    return "abi3-macos-arm64";
#else
    return "abi3-macos-x64";
#endif
#else
    return "abi3-linux-x64";
#endif
}

std::string cart_artifact_library_name() {
#if defined(_WIN32)
    return "cart.dll";
#elif defined(__APPLE__)
    return "libcart.dylib";
#else
    return "libcart.so";
#endif
}

std::filesystem::path cart_artifact_dir(const std::string& rom_sha1) {
    return user_data_dir() / "cart_aot" / rom_sha1 / cart_artifact_abi_tag();
}

std::filesystem::path cart_artifact_path(const std::string& rom_sha1) {
    return cart_artifact_dir(rom_sha1) / cart_artifact_library_name();
}

}
