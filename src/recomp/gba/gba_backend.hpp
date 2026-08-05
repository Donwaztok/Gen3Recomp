#pragma once

#include "game_definition.hpp"
#include "session_backend.hpp"

#include <filesystem>

namespace gen3recomp {

class GbaSessionBackend final : public SessionBackend {
public:
    GbaSessionBackend(
        std::filesystem::path rom_path,
        std::filesystem::path bios_path,
        std::filesystem::path cache_dir,
        GameDefinition game);

    bool start() override;
    Frame step() override;
    void stop() override;

    const std::filesystem::path& cache_dir() const { return cache_dir_; }

private:
    std::filesystem::path rom_path_;
    std::filesystem::path bios_path_;
    std::filesystem::path cache_dir_;
    GameDefinition game_;
    bool started_ = false;
};

}
