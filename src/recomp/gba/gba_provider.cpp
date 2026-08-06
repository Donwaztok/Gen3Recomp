#include "gba_provider.hpp"

#include "cart_artifact.hpp"
#include "gba_backend.hpp"
#include "mods.hpp"
#include "user_data.hpp"

#include <algorithm>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <system_error>

namespace gen3recomp {

bool GbaRecompProvider::prepare(
    const std::filesystem::path& rom_path,
    const std::filesystem::path& bios_path,
    const GameDefinition& game,
    PreparedSession& out,
    std::string& error,
    const std::vector<std::string>& enabled_mods) const {
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

    // Fail closed: enabled ids must still be valid discoverable packages.
    const auto discovered = discover_mods();
    for (const auto& id : enabled_mods) {
        const auto it = std::find_if(
            discovered.begin(),
            discovered.end(),
            [&](const ModPackage& m) { return m.id == id; });
        if (it == discovered.end()) {
            error = "enabled mod not found: " + id;
            return false;
        }
        if (!it->valid) {
            error = "enabled mod is invalid: " + id + " (" + it->invalid_reason + ")";
            return false;
        }
        // v1: no hook kinds supported yet — packages that declare hooks are already invalid.
        spdlog::info("prepare: enabled mod id={} name={}", it->id, it->name);
    }

    // Prefer activating a user-data cart artifact on stock (stub-linked) hosts.
    // Link-time carts skip dlopen inside try_activate_cart_artifact.
    if (cart_artifact_ready(game.sha1)) {
        if (!try_activate_cart_artifact(game.sha1, error)) {
            return false;
        }
    }

    const auto cache_dir = recomp_cache_dir(game.sha1);
    const auto save_path = cartridge_save_path(game.sha1);
    std::error_code create_error;
    std::filesystem::create_directories(cache_dir, create_error);
    if (create_error) {
        error = "failed to create recomp cache directory: " + create_error.message();
        return false;
    }
    std::filesystem::create_directories(save_path.parent_path(), create_error);
    if (create_error) {
        error = "failed to create save directory: " + create_error.message();
        return false;
    }

    out.backend = std::make_unique<GbaSessionBackend>(
        std::filesystem::weakly_canonical(rom_path),
        std::filesystem::weakly_canonical(bios_path),
        cache_dir,
        save_path,
        game);
    return true;
}

}
