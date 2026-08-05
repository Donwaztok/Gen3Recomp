#pragma once

#include "game_definition.hpp"
#include "session_backend.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace gen3recomp {

struct PreparedSession {
    std::unique_ptr<SessionBackend> backend;
};

class RecompilerProvider {
public:
    virtual ~RecompilerProvider() = default;
    virtual bool prepare(
        const std::filesystem::path& rom_path,
        const std::filesystem::path& bios_path,
        const GameDefinition& game,
        PreparedSession& out,
        std::string& error) const = 0;
};

}
