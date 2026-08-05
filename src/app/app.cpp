#include "app.hpp"

#include "cli.hpp"
#include "logging.hpp"
#include "version.hpp"

#include <cstdio>
#include <spdlog/spdlog.h>

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
        if (parsed.code == gen3recomp::ExitCode::InputError) {
            spdlog::error("{}", parsed.message);
        }
        return static_cast<int>(parsed.code);
    }

    spdlog::info("ROM: {}", *parsed.request.rom_path);
    std::printf("ROM: %s\n", parsed.request.rom_path->c_str());
    if (parsed.request.bios_path.has_value()) {
        spdlog::info("BIOS: {}", *parsed.request.bios_path);
        std::printf("BIOS: %s\n", parsed.request.bios_path->c_str());
    }

    return static_cast<int>(gen3recomp::ExitCode::Ok);
}
