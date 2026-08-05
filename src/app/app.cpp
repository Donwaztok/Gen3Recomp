#include "app.hpp"

#include "audio.hpp"
#include "catalog.hpp"
#include "cli.hpp"
#include "file_bytes.hpp"
#include "logging.hpp"
#include "gba_provider.hpp"
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

namespace {

std::optional<std::filesystem::path> resolve_bios_path(const gen3recomp::LaunchRequest& request) {
    if (request.bios_path.has_value()) {
        return std::filesystem::path{*request.bios_path};
    }
    const std::filesystem::path local{"gba_bios.bin"};
    if (std::filesystem::is_regular_file(local)) {
        return local;
    }
    return std::nullopt;
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

    const auto rom_sha1 = sha1_file(*parsed.request.rom_path);
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

    const auto bios_path = resolve_bios_path(parsed.request);
    if (!bios_path.has_value()) {
        const char* message =
            "error: a GBA BIOS is required (--bios <path> or ./gba_bios.bin)\n";
        std::fputs(message, stderr);
        spdlog::error("missing GBA BIOS");
        return static_cast<int>(gen3recomp::ExitCode::InputError);
    }

    const auto bios_sha1 = sha1_file(*bios_path);
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

    if (parsed.request.prepare_cache) {
        const std::string frames = std::to_string(parsed.request.prepare_frames);
#if defined(_WIN32)
        _putenv_s("GEN3RECOMP_PREPARE_FRAMES", frames.c_str());
#else
        setenv("GEN3RECOMP_PREPARE_FRAMES", frames.c_str(), 1);
#endif
        std::printf(
            "Preparing native-code cache for %d guest frames, then exiting.\n"
            "Cache: ~/.local/share/gen3recomp/recomp_cache/%s/\n"
            "Launch again without --prepare to play at full speed.\n",
            parsed.request.prepare_frames,
            rom_sha1.c_str());
        spdlog::info("prepare-cache frames={}", parsed.request.prepare_frames);
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
        if (!provider.prepare(*parsed.request.rom_path, *bios_path, *game, prepared, prepare_error)) {
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
