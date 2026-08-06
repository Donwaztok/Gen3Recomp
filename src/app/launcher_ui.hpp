#pragma once

#include "exit_codes.hpp"

#include <filesystem>
#include <optional>
#include <string>

namespace gen3recomp {

struct LauncherResult {
    ExitCode code = ExitCode::Ok;
    bool play = false;
    std::optional<std::filesystem::path> rom_path;
    std::optional<std::filesystem::path> bios_path;
    std::string message;
};

// Blocking SDL3 launcher. Returns play=true when the user activates Play with
// a catalogued dump, ready cart artifact, and valid BIOS.
LauncherResult run_launcher_ui(
    const std::optional<std::filesystem::path>& preferred_bios,
    const std::optional<std::filesystem::path>& exe_dir);

}  // namespace gen3recomp
