#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace gen3recomp {

struct ModPackage {
    std::string id;
    std::string name;
    std::string version;
    std::filesystem::path root;
    bool enabled = false;
    bool valid = true;
    std::string invalid_reason;
};

// Discovery roots: <user_data>/mods and ./mods (CWD).
std::vector<std::filesystem::path> mod_discovery_roots();

std::vector<ModPackage> discover_mods();
bool set_mod_enabled(const std::string& id, bool enabled, std::string& error);
std::vector<std::string> enabled_mod_ids();

// Persist enablement under user data.
void load_mod_enablement(std::vector<ModPackage>& mods);
bool save_mod_enablement(const std::vector<ModPackage>& mods, std::string& error);

}  // namespace gen3recomp
