#include "cli.hpp"

#include <cstdlib>
#include <filesystem>
#include <string_view>

namespace gen3recomp {
namespace {

bool file_exists(const std::string& path) {
    std::error_code error;
    return std::filesystem::is_regular_file(path, error);
}

}  // namespace

std::string usage_text() {
    return "Usage: gen3recomp [--rom <path>] [--bios <path>] [--prepare [frames]]\n"
           "       gen3recomp --help\n"
           "       gen3recomp --version\n"
           "\n"
           "With no --rom, opens the launcher UI (ROM list, AOT Build/Play, mods).\n"
           "\n"
           "Options:\n"
           "  --rom <path>         Path to a legally obtained GBA ROM (CLI path)\n"
           "  --bios <path>        Path to a legally obtained GBA BIOS\n"
           "                       (or ./gba_bios.bin in the working directory)\n"
           "  --prepare [frames]   Optional diagnostic: warm self-heal cache only\n"
           "                       (not full-cart AOT; see README)\n"
           "  --help               Show this help text\n"
           "  --version            Print version identity\n"
           "\n"
           "Full-speed play: build local cart AOT once (launcher Build, or)\n"
           "  ./scripts/build_cart_artifact.sh <rom.gba>\n"
           "Stock hosts dlopen the resulting libcart.so — no CMake relink required.\n"
           "\n"
           "Controls (in-game):\n"
           "  Arrows  D-pad     X  A     Z  B\n"
           "  Enter   Start     Right Shift  Select\n"
           "  C       L         V  R     Esc/Q  quit\n"
           "\n"
           "Exit codes:\n"
           "  0  success\n"
           "  1  input error (missing file or invalid path)\n"
           "  2  usage error (unknown option)\n";
}

ParseResult parse_args(int argc, char** argv) {
    ParseResult result;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg{argv[i]};

        if (arg == "--help") {
            result.request.show_help = true;
            result.message = usage_text();
            return result;
        }

        if (arg == "--version") {
            result.request.show_version = true;
            return result;
        }

        if (arg == "--rom") {
            if (i + 1 >= argc) {
                result.code = ExitCode::UsageError;
                result.message = "error: --rom requires a file path\n\n" + usage_text();
                return result;
            }
            result.request.rom_path = argv[++i];
            continue;
        }

        if (arg == "--bios") {
            if (i + 1 >= argc) {
                result.code = ExitCode::UsageError;
                result.message = "error: --bios requires a file path\n\n" + usage_text();
                return result;
            }
            result.request.bios_path = argv[++i];
            continue;
        }

        if (arg == "--prepare") {
            result.request.prepare_cache = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                char* end = nullptr;
                const long parsed = std::strtol(argv[i + 1], &end, 10);
                if (end == argv[i + 1] || *end != '\0' || parsed < 1 || parsed > 1000000) {
                    result.code = ExitCode::UsageError;
                    result.message = "error: --prepare requires a frame count between 1 and 1000000\n\n" +
                                     usage_text();
                    return result;
                }
                result.request.prepare_frames = static_cast<int>(parsed);
                ++i;
            }
            continue;
        }

        result.code = ExitCode::UsageError;
        result.message = "error: unknown option '" + std::string(arg) + "'\n\n" + usage_text();
        return result;
    }

    if (result.request.show_help || result.request.show_version) {
        return result;
    }

    if (!result.request.rom_path.has_value()) {
        result.request.open_launcher = true;
        return result;
    }

    if (!file_exists(*result.request.rom_path)) {
        result.code = ExitCode::InputError;
        result.message = "error: ROM file not found: " + *result.request.rom_path + "\n";
        return result;
    }

    if (result.request.bios_path.has_value() && !file_exists(*result.request.bios_path)) {
        result.code = ExitCode::InputError;
        result.message = "error: BIOS file not found: " + *result.request.bios_path + "\n";
        return result;
    }

    return result;
}

}  // namespace gen3recomp
