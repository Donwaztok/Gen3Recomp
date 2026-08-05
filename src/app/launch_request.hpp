#pragma once

#include <optional>
#include <string>

namespace gen3recomp {

struct LaunchRequest {
    bool show_help = false;
    bool show_version = false;
    std::optional<std::string> rom_path;
    std::optional<std::string> bios_path;
};

}
