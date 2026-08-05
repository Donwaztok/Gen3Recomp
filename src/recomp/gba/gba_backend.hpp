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
        std::filesystem::path save_path,
        GameDefinition game);

    bool start() override;
    Frame step() override;
    void stop() override;
    bool owns_host_loop() const override { return true; }

    const std::filesystem::path& cache_dir() const { return cache_dir_; }
    const std::filesystem::path& save_path() const { return save_path_; }

private:
    std::filesystem::path rom_path_;
    std::filesystem::path bios_path_;
    std::filesystem::path cache_dir_;
    std::filesystem::path save_path_;
    GameDefinition game_;
    bool started_ = false;
};

}
