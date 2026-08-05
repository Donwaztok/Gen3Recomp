#include "gba_backend.hpp"

#if defined(GEN3RECOMP_HAS_GBARECOMP)
#include "runtime.h"
#endif

#include <cstdlib>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace gen3recomp {
namespace {

void set_env(const char* key, const char* value) {
#if defined(_WIN32)
    _putenv_s(key, value);
#else
    setenv(key, value, 1);
#endif
}

}  // namespace

GbaSessionBackend::GbaSessionBackend(
    std::filesystem::path rom_path,
    std::filesystem::path bios_path,
    std::filesystem::path cache_dir,
    std::filesystem::path save_path,
    GameDefinition game)
    : rom_path_(std::move(rom_path)),
      bios_path_(std::move(bios_path)),
      cache_dir_(std::move(cache_dir)),
      save_path_(std::move(save_path)),
      game_(std::move(game)) {}

bool GbaSessionBackend::start() {
#if !defined(GEN3RECOMP_HAS_GBARECOMP)
    spdlog::error("gba-recomp runtime is not linked; init the third_party/gbarecomp submodule");
    return false;
#else
    started_ = true;
    spdlog::info(
        "gba-recomp adapter launching id={} cache={} save={} rom={} bios={}",
        game_.id,
        cache_dir_.string(),
        save_path_.string(),
        rom_path_.string(),
        bios_path_.string());

    std::error_code cwd_error;
    const auto previous_cwd = std::filesystem::current_path(cwd_error);
    const auto user_root = cache_dir_.parent_path().parent_path();
    std::filesystem::current_path(user_root, cwd_error);

    set_env("GBARECOMP_NO_LAUNCHER", "1");
    if (std::getenv("GEN3RECOMP_DISABLE_SELFHEAL") != nullptr) {
        unsetenv("GBARECOMP_SELFHEAL_RECOMPILE");
    } else {
        set_env("GBARECOMP_SELFHEAL_RECOMPILE", "1");
    }

    std::vector<std::string> args{
        "gen3recomp",
        "--rom",
        rom_path_.string(),
        "--bios",
        bios_path_.string(),
        "--save",
        save_path_.string(),
        "--scale",
        "4",
    };
    if (const char* frames = std::getenv("GEN3RECOMP_HEADLESS_FRAMES");
        frames != nullptr && *frames != '\0' && std::string{frames} != "0") {
        args.insert(args.end(), {"--frames", frames, "--no-window"});
    }

    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }

    gbarecomp::RunOptions opts;
    opts.builtin_game_name = game_.display_name.c_str();
    opts.builtin_rom_sha1 = game_.sha1.c_str();
    opts.launcher_save_path = save_path_.c_str();

    const int rc = gbarecomp::run_game(static_cast<int>(argv.size()), argv.data(), opts);
    if (!previous_cwd.empty()) {
        std::filesystem::current_path(previous_cwd, cwd_error);
    }
    started_ = false;
    if (rc != 0) {
        spdlog::error("gba-recomp run_game exited with {}", rc);
        return false;
    }
    return true;
#endif
}

Frame GbaSessionBackend::step() {
    return {};
}

void GbaSessionBackend::stop() {
    started_ = false;
}

}
