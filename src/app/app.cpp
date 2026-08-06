#include "app.hpp"

#include "audio.hpp"
#include "catalog.hpp"
#include "cli.hpp"
#include "file_bytes.hpp"
#include "launcher_ui.hpp"
#include "logging.hpp"
#include "gba_provider.hpp"
#include "mods.hpp"
#include "null_backend.hpp"
#include "platform.hpp"
#include "session.hpp"
#include "version.hpp"
#include "video.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#if !defined(_WIN32)
#include <unistd.h>
#endif

namespace {

std::optional<std::filesystem::path> resolve_bios_path(
    const gen3recomp::LaunchRequest& request,
    const std::optional<std::filesystem::path>& exe_dir) {
    if (request.bios_path.has_value()) {
        return std::filesystem::path{*request.bios_path};
    }
    std::vector<std::filesystem::path> candidates;
    if (exe_dir.has_value()) {
        if (exe_dir->filename() == "bin") {
            candidates.push_back(exe_dir->parent_path() / "gba_bios.bin");
        }
        candidates.push_back(*exe_dir / "gba_bios.bin");
    }
    candidates.emplace_back("gba_bios.bin");
    for (const auto& candidate : candidates) {
        if (std::filesystem::is_regular_file(candidate)) {
            return candidate;
        }
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> exe_parent_dir(char* argv0) {
    if (argv0 == nullptr || *argv0 == '\0') {
        return std::nullopt;
    }
    std::error_code error;
    auto path = std::filesystem::absolute(argv0, error);
    if (error) {
        return std::nullopt;
    }
    return path.parent_path();
}

std::optional<std::filesystem::path> find_tauri_launcher(char* argv0) {
    if (const char* override_path = std::getenv("GEN3RECOMP_LAUNCHER");
        override_path != nullptr && *override_path != '\0') {
        std::filesystem::path p{override_path};
        if (std::filesystem::is_regular_file(p)) {
            return p;
        }
    }

    std::vector<std::filesystem::path> candidates;
#if defined(_WIN32)
    const char* name = "gen3recomp-launcher.exe";
#else
    const char* name = "gen3recomp-launcher";
#endif
    if (const auto parent = exe_parent_dir(argv0); parent.has_value()) {
        candidates.push_back(*parent / name);
        candidates.push_back(*parent / "launcher" / name);
    }
    candidates.emplace_back(name);
    candidates.push_back(std::filesystem::path{"launcher"} / "src-tauri" / "target" / "release" / name);
    candidates.push_back(std::filesystem::path{"launcher"} / "src-tauri" / "target" / "debug" / name);

    for (const auto& c : candidates) {
        std::error_code ec;
        if (std::filesystem::is_regular_file(c, ec)) {
            return std::filesystem::absolute(c, ec);
        }
    }
    return std::nullopt;
}

int try_exec_tauri_launcher(char* argv0) {
    const auto launcher = find_tauri_launcher(argv0);
    if (!launcher.has_value()) {
        std::fputs(
            "Player UI: gen3recomp-launcher not found.\n"
            "Build it: cd launcher && npm install && npm run tauri:build\n"
            "Or set GEN3RECOMP_LAUNCHER, or use --rom <path> for CLI play.\n"
            "Legacy SDL launcher: set GEN3RECOMP_SDL_LAUNCHER=1\n",
            stderr);
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }

#if defined(_WIN32)
    const std::string cmd = "\"" + launcher->string() + "\"";
    const int rc = std::system(cmd.c_str());
    return rc == 0 ? static_cast<int>(gen3recomp::ExitCode::Ok)
                   : static_cast<int>(gen3recomp::ExitCode::InputError);
#else
    std::string path = launcher->string();
    char* arg0 = path.data();
    char* args[] = {arg0, nullptr};
    execv(path.c_str(), args);
    std::perror("execv gen3recomp-launcher");
    return static_cast<int>(gen3recomp::ExitCode::InputError);
#endif
}

int run_session_for_rom(
    const std::filesystem::path& rom_path,
    const std::filesystem::path& bios_path,
    bool prepare_cache,
    int prepare_frames) {
    const auto rom_sha1 = sha1_file(rom_path);
    const auto catalog = gen3recomp::Catalog::builtin();
    const auto game = catalog.find_by_sha1(rom_sha1);
    if (!game.has_value()) {
        const std::string message =
            "error: unsupported ROM dump\n"
            "SHA-1: " +
            rom_sha1 +
            "\nMVP supports USA Ruby, Sapphire, and Emerald only.\n";
        std::fputs(message.c_str(), stderr);
        spdlog::error("unsupported ROM SHA-1 {}", rom_sha1);
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }

    const auto bios_sha1 = sha1_file(bios_path);
    if (!gen3recomp::is_known_bios_sha1(bios_sha1)) {
        const std::string message =
            "error: unsupported GBA BIOS\n"
            "SHA-1: " +
            bios_sha1 + "\n";
        std::fputs(message.c_str(), stderr);
        spdlog::error("unsupported BIOS SHA-1 {}", bios_sha1);
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }

    std::printf("Identified: %s (%s)\n", game->display_name.c_str(), game->region.c_str());
    std::printf("ROM SHA-1: %s\n", rom_sha1.c_str());
    std::printf("BIOS SHA-1: %s\n", bios_sha1.c_str());
    spdlog::info("identified {} ({}) sha1={}", game->display_name, game->region, rom_sha1);

    if (prepare_cache) {
        const std::string frames = std::to_string(prepare_frames);
#if defined(_WIN32)
        _putenv_s("GEN3RECOMP_PREPARE_FRAMES", frames.c_str());
#else
        setenv("GEN3RECOMP_PREPARE_FRAMES", frames.c_str(), 1);
#endif
        std::printf(
            "Preparing native-code cache for %d guest frames, then exiting.\n"
            "Cache: ~/.local/share/gen3recomp/recomp_cache/%s/\n"
            "Launch again without --prepare to play at full speed.\n",
            prepare_frames,
            rom_sha1.c_str());
        spdlog::info("prepare-cache frames={}", prepare_frames);
    }

    std::unique_ptr<gen3recomp::SessionBackend> owned_backend;
    gen3recomp::SessionBackend* backend = nullptr;
    gen3recomp::NullBackend null_backend;
    if (const char* use_null = std::getenv("GEN3RECOMP_NULL_BACKEND");
        use_null != nullptr && *use_null != '\0' && std::string{use_null} != "0") {
        backend = &null_backend;
        spdlog::info("using null session backend");
    } else {
        gen3recomp::GbaRecompProvider provider;
        gen3recomp::PreparedSession prepared;
        std::string prepare_error;
        const auto mods = gen3recomp::enabled_mod_ids();
        if (!provider.prepare(rom_path, bios_path, *game, prepared, prepare_error, mods)) {
            const std::string message = "error: recompiler provider failed: " + prepare_error + "\n";
            std::fputs(message.c_str(), stderr);
            spdlog::error("{}", prepare_error);
            return static_cast<int>(gen3recomp::ExitCode::InputError);
        }
        owned_backend = std::move(prepared.backend);
        backend = owned_backend.get();
    }

    gen3recomp::Session session;
    if (backend->owns_host_loop()) {
        const int session_rc = session.run(*backend);
        if (session_rc != 0) {
            std::fputs("error: runtime session failed\n", stderr);
            return static_cast<int>(gen3recomp::ExitCode::InputError);
        }
        return static_cast<int>(gen3recomp::ExitCode::Ok);
    }

    const std::string title = std::string{"gen3recomp — "} + game->display_name;
    constexpr int kScale = 4;
    gen3recomp::Platform platform;
    if (!platform.init(
            title.c_str(),
            gen3recomp::window_size_for_scale(gen3recomp::kGbaWidth, kScale),
            gen3recomp::window_size_for_scale(gen3recomp::kGbaHeight, kScale))) {
        std::fputs("error: failed to open the host window\n", stderr);
        spdlog::error("platform init failed");
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }

    gen3recomp::AudioDevice audio;
    if (!audio.init()) {
        spdlog::error("continuing without host audio");
    }

    const int session_rc = session.run(platform, *backend, &audio);
    audio.shutdown();
    platform.shutdown();
    if (session_rc != 0) {
        std::fputs("error: runtime session failed\n", stderr);
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }
    return static_cast<int>(gen3recomp::ExitCode::Ok);
}

}  // namespace

int run_app(int argc, char** argv) {
    init_logging();

    const auto parsed = gen3recomp::parse_args(argc, argv);

    if (parsed.request.show_help) {
        std::fputs(parsed.message.c_str(), stdout);
        return static_cast<int>(gen3recomp::ExitCode::Ok);
    }

    if (parsed.request.show_version) {
        std::printf("gen3recomp %s\n", GEN3RECOMP_VERSION);
        return static_cast<int>(gen3recomp::ExitCode::Ok);
    }

    if (parsed.code != gen3recomp::ExitCode::Ok) {
        std::fputs(parsed.message.c_str(), stderr);
        return static_cast<int>(parsed.code);
    }

    if (parsed.request.open_launcher) {
        if (const char* sdl = std::getenv("GEN3RECOMP_SDL_LAUNCHER");
            sdl != nullptr && *sdl != '\0' && std::string{sdl} != "0") {
            const auto exe_dir = exe_parent_dir(argv[0]);
            const auto preferred_bios = resolve_bios_path(parsed.request, exe_dir);
            const auto launcher = gen3recomp::run_launcher_ui(preferred_bios, exe_dir);
            if (!launcher.play) {
                if (!launcher.message.empty()) {
                    std::fputs(launcher.message.c_str(), stderr);
                }
                return static_cast<int>(launcher.code);
            }
            if (!launcher.rom_path.has_value() || !launcher.bios_path.has_value()) {
                std::fputs("error: launcher Play missing ROM or BIOS path\n", stderr);
                return static_cast<int>(gen3recomp::ExitCode::InputError);
            }
            return run_session_for_rom(
                *launcher.rom_path,
                *launcher.bios_path,
                false,
                3600);
        }
        return try_exec_tauri_launcher(argv[0]);
    }

    const auto bios_path = resolve_bios_path(parsed.request, exe_parent_dir(argv[0]));
    if (!bios_path.has_value()) {
        const char* message =
            "error: a GBA BIOS is required (--bios <path>, package-root gba_bios.bin, "
            "or ./gba_bios.bin)\n";
        std::fputs(message, stderr);
        spdlog::error("missing GBA BIOS");
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }

    return run_session_for_rom(
        *parsed.request.rom_path,
        *bios_path,
        parsed.request.prepare_cache,
        parsed.request.prepare_frames);
}
