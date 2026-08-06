#pragma once

#include <filesystem>
#include <functional>
#include <string>

namespace gen3recomp {

using CartBuildLogFn = std::function<void(const std::string& line)>;

// Resolve the repo/install root that contains scripts/build_cart_artifact.sh.
std::filesystem::path resolve_project_root();

// Run the same pipeline as scripts/build_cart_artifact.sh for the given ROM.
// Streams stdout/stderr lines to on_log when provided. On failure, error is set.
bool build_cart_artifact(
    const std::filesystem::path& rom_path,
    CartBuildLogFn on_log,
    std::string& error);

}  // namespace gen3recomp
