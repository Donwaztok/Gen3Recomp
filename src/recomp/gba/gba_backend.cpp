#include "gba_backend.hpp"

#if defined(GEN3RECOMP_HAS_GBARECOMP)
#include "runtime.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <fstream>
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

std::string toml_escape(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (char ch : value) {
        if (ch == '\\' || ch == '"') {
            out.push_back('\\');
        }
        out.push_back(ch);
    }
    return out;
}

bool write_game_config(
    const std::filesystem::path& path,
    const std::filesystem::path& rom_path,
    const std::filesystem::path& bios_path,
    const std::filesystem::path& save_path,
    const GameDefinition& game) {
    std::ofstream out{path, std::ios::trunc};
    if (!out) {
        return false;
    }
    out << "[game]\n"
        << "name = \"" << toml_escape(game.display_name) << "\"\n"
        << "short_name = \"" << toml_escape(game.id) << "\"\n\n"
        << "[rom]\n"
        << "path = \"" << toml_escape(rom_path.string()) << "\"\n"
        << "sha1 = \"" << game.sha1 << "\"\n\n"
        << "[bios]\n"
        << "path = \"" << toml_escape(bios_path.string()) << "\"\n"
        << "hle = false\n\n"
        << "[save]\n"
        << "path = \"" << toml_escape(save_path.string()) << "\"\n"
        << "type = \"" << toml_escape(game.save_family) << "\"\n\n"
        << "[runtime]\n"
        << "window_title = \"gen3recomp — " << toml_escape(game.display_name) << "\"\n";
    return static_cast<bool>(out);
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

    const auto config_path = cache_dir_ / "game.toml";
    if (!write_game_config(config_path, rom_path_, bios_path_, save_path_, game_)) {
        spdlog::error("failed to write {}", config_path.string());
        started_ = false;
        return false;
    }

    std::error_code cwd_error;
    const auto previous_cwd = std::filesystem::current_path(cwd_error);
    const auto user_root = cache_dir_.parent_path().parent_path();
    std::filesystem::current_path(user_root, cwd_error);

    setvbuf(stdout, nullptr, _IONBF, 0);
    set_env("GBARECOMP_NO_LAUNCHER", "1");
    // SDL2 OpenGL + RGB24 streaming textures present as a blank white frame on
    // some NVIDIA/Wayland hosts. Software is correct; users can override.
    if (std::getenv("SDL_RENDER_DRIVER") == nullptr) {
        set_env("SDL_RENDER_DRIVER", "software");
    }
    if (!game_.save_family.empty()) {
        set_env("GBARECOMP_SAVE_TYPE", game_.save_family.c_str());
    }
    if (std::getenv("GEN3RECOMP_SELFHEAL") != nullptr) {
        set_env("GBARECOMP_SELFHEAL_RECOMPILE", "1");
    } else if (std::getenv("GEN3RECOMP_DISABLE_SELFHEAL") != nullptr) {
        unsetenv("GBARECOMP_SELFHEAL_RECOMPILE");
    } else {
        set_env("GBARECOMP_SELFHEAL_RECOMPILE", "1");
    }
    // Gen3 copies hot code (IRQ / m4a / AgbMain) into IWRAM. Without RAM
    // overlay heal those PCs are marked failed and stay on the interpreter,
    // so boot never leaves forced-blank white after the BIOS logos.
    if (std::getenv("GBARECOMP_RAM_OVERLAY_HEAL") == nullptr &&
        std::getenv("GEN3RECOMP_DISABLE_RAM_HEAL") == nullptr) {
        set_env("GBARECOMP_RAM_OVERLAY_HEAL", "1");
    }
    if (std::getenv("GBARECOMP_SYNC_OVERLAY_HEAL") == nullptr &&
        std::getenv("GEN3RECOMP_DISABLE_SYNC_HEAL") == nullptr) {
        set_env("GBARECOMP_SYNC_OVERLAY_HEAL", "1");
    }
    if (std::getenv("GBARECOMP_HANG_WATCHDOG") == nullptr) {
        set_env("GBARECOMP_HANG_WATCHDOG", "0");
    }

    std::vector<std::string> args{
        "gen3recomp",
        "--config",
        config_path.string(),
        "--rom",
        rom_path_.string(),
        "--bios",
        bios_path_.string(),
        "--save",
        save_path_.string(),
        "--scale",
        "4",
        "--no-bios-hle",
    };
    if (const char* frames = std::getenv("GEN3RECOMP_PREPARE_FRAMES");
        frames != nullptr && *frames != '\0' && std::string{frames} != "0") {
        args.insert(args.end(), {"--frames", frames});
        spdlog::info("prepare-cache running {} windowed guest frames", frames);
    } else if (const char* frames = std::getenv("GEN3RECOMP_HEADLESS_FRAMES");
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

    spdlog::info("upstream config={} save_type={}", config_path.string(), game_.save_family);
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
