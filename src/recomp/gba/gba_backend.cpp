#include "gba_backend.hpp"

#include "video.hpp"

#include <spdlog/spdlog.h>

namespace gen3recomp {

GbaSessionBackend::GbaSessionBackend(
    std::filesystem::path rom_path,
    std::filesystem::path bios_path,
    std::filesystem::path cache_dir,
    GameDefinition game)
    : rom_path_(std::move(rom_path)),
      bios_path_(std::move(bios_path)),
      cache_dir_(std::move(cache_dir)),
      game_(std::move(game)) {}

bool GbaSessionBackend::start() {
    started_ = true;
    spdlog::info(
        "gba-recomp adapter ready id={} cache={} rom={} bios={}",
        game_.id,
        cache_dir_.string(),
        rom_path_.string(),
        bios_path_.string());
    return true;
}

Frame GbaSessionBackend::step() {
    Frame frame;
    frame.width = kGbaWidth;
    frame.height = kGbaHeight;
    frame.pixels.resize(static_cast<std::size_t>(kGbaWidth * kGbaHeight));
    fill_placeholder_frame(frame.pixels, kGbaWidth, kGbaHeight);
    return frame;
}

void GbaSessionBackend::stop() {
    started_ = false;
}

}
