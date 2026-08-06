#include "mods.hpp"

#include "user_data.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <spdlog/spdlog.h>
#include <unordered_set>

namespace gen3recomp {
namespace {

std::filesystem::path mods_config_path() {
    return user_data_dir() / "mods_enabled.txt";
}

std::string trim(std::string value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
        value.erase(value.begin());
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
        value.pop_back();
    }
    return value;
}

std::string unquote(std::string value) {
    value = trim(std::move(value));
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

bool parse_mod_toml(const std::filesystem::path& path, ModPackage& out, std::string& error) {
    std::ifstream in{path};
    if (!in) {
        error = "cannot read mod.toml";
        return false;
    }
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '[') {
            continue;
        }
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        const std::string key = trim(line.substr(0, eq));
        const std::string value = unquote(line.substr(eq + 1));
        if (key == "name") {
            out.name = value;
        } else if (key == "version") {
            out.version = value;
        } else if (key == "hooks" || key == "hook") {
            // v1: any declared hook kind other than empty is unsupported → fail closed later
            if (!value.empty() && value != "[]" && value != "none") {
                out.valid = false;
                out.invalid_reason = "unsupported hook kind in mod.toml (v1 supports enable flags only)";
            }
        }
    }
    if (out.name.empty()) {
        out.name = out.id;
    }
    if (out.version.empty()) {
        out.version = "0";
    }
    return true;
}

bool package_contains_forbidden_payload(const std::filesystem::path& root, std::string& reason) {
    std::error_code error;
    if (!std::filesystem::is_directory(root, error)) {
        return false;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root, error)) {
        if (error) {
            break;
        }
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto ext = entry.path().extension().string();
        std::string lower = ext;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        const auto name = entry.path().filename().string();
        std::string name_lower = name;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        if (lower == ".gba" || lower == ".agb" || name_lower == "gba_bios.bin" ||
            (name_lower.find("bios") != std::string::npos && lower == ".bin")) {
            reason = "package embeds ROM/BIOS payload: " + entry.path().filename().string();
            return true;
        }
    }
    return false;
}

std::unordered_set<std::string> read_enabled_ids() {
    std::unordered_set<std::string> enabled;
    std::ifstream in{mods_config_path()};
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        enabled.insert(line);
    }
    return enabled;
}

}  // namespace

std::vector<std::filesystem::path> mod_discovery_roots() {
    return {user_data_dir() / "mods", std::filesystem::path{"mods"}};
}

std::vector<ModPackage> discover_mods() {
    std::vector<ModPackage> mods;
    std::unordered_set<std::string> seen;
    for (const auto& root : mod_discovery_roots()) {
        std::error_code error;
        if (!std::filesystem::is_directory(root, error)) {
            continue;
        }
        for (const auto& entry : std::filesystem::directory_iterator(root, error)) {
            if (error || !entry.is_directory()) {
                continue;
            }
            ModPackage mod;
            mod.id = entry.path().filename().string();
            if (mod.id.empty() || !seen.insert(mod.id).second) {
                continue;
            }
            mod.root = entry.path();
            const auto manifest = entry.path() / "mod.toml";
            if (!std::filesystem::is_regular_file(manifest, error)) {
                continue;
            }
            std::string parse_error;
            if (!parse_mod_toml(manifest, mod, parse_error)) {
                mod.valid = false;
                mod.invalid_reason = parse_error;
            }
            std::string forbid_reason;
            if (package_contains_forbidden_payload(mod.root, forbid_reason)) {
                mod.valid = false;
                mod.invalid_reason = forbid_reason;
            }
            mods.push_back(std::move(mod));
        }
    }
    std::sort(mods.begin(), mods.end(), [](const ModPackage& a, const ModPackage& b) {
        return a.id < b.id;
    });
    load_mod_enablement(mods);
    return mods;
}

void load_mod_enablement(std::vector<ModPackage>& mods) {
    const auto enabled = read_enabled_ids();
    for (auto& mod : mods) {
        mod.enabled = mod.valid && enabled.count(mod.id) > 0;
    }
}

bool save_mod_enablement(const std::vector<ModPackage>& mods, std::string& error) {
    std::error_code create_error;
    std::filesystem::create_directories(user_data_dir(), create_error);
    if (create_error) {
        error = create_error.message();
        return false;
    }
    std::ofstream out{mods_config_path(), std::ios::trunc};
    if (!out) {
        error = "cannot write " + mods_config_path().string();
        return false;
    }
    out << "# gen3recomp enabled mod ids (one per line)\n";
    for (const auto& mod : mods) {
        if (mod.enabled && mod.valid) {
            out << mod.id << '\n';
        }
    }
    return static_cast<bool>(out);
}

bool set_mod_enabled(const std::string& id, bool enabled, std::string& error) {
    auto mods = discover_mods();
    auto it = std::find_if(mods.begin(), mods.end(), [&](const ModPackage& m) {
        return m.id == id;
    });
    if (it == mods.end()) {
        error = "unknown mod id: " + id;
        return false;
    }
    if (enabled && !it->valid) {
        error = it->invalid_reason.empty() ? "mod is not enableable" : it->invalid_reason;
        return false;
    }
    it->enabled = enabled;
    return save_mod_enablement(mods, error);
}

std::vector<std::string> enabled_mod_ids() {
    std::vector<std::string> ids;
    for (const auto& mod : discover_mods()) {
        if (mod.enabled && mod.valid) {
            ids.push_back(mod.id);
        }
    }
    return ids;
}

}  // namespace gen3recomp
