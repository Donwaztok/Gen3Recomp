#pragma once

#include "recomp_provider.hpp"

namespace gen3recomp {

class GbaRecompProvider final : public RecompilerProvider {
public:
    bool prepare(
        const std::filesystem::path& rom_path,
        const std::filesystem::path& bios_path,
        const GameDefinition& game,
        PreparedSession& out,
        std::string& error,
        const std::vector<std::string>& enabled_mods = {}) const override;
};

}
