#include "cli.hpp"

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
    return "Usage: gen3recomp --rom <path> [--bios <path>]\n"
           "       gen3recomp --help\n"
           "       gen3recomp --version\n"
           "\n"
           "Options:\n"
           "  --rom <path>    Path to a legally obtained GBA ROM (required)\n"
           "  --bios <path>   Path to a legally obtained GBA BIOS (required to continue)\n"
           "  --help          Show this help text\n"
           "  --version       Print version identity\n"
           "\n"
           "Controls:\n"
           "  Arrows  D-pad     X  A     Z  B\n"
           "  Enter   Start     Right Shift  Select\n"
           "  C       L         V  R     Esc/Q  quit\n"
           "\n"
           "Exit codes:\n"
           "  0  success\n"
           "  1  input error (missing file or invalid path)\n"
           "  2  usage error (unknown option or missing --rom)\n";
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

        result.code = ExitCode::UsageError;
        result.message = "error: unknown option '" + std::string(arg) + "'\n\n" + usage_text();
        return result;
    }

    if (result.request.show_help || result.request.show_version) {
        return result;
    }

    if (!result.request.rom_path.has_value()) {
        result.code = ExitCode::UsageError;
        result.message = "error: --rom is required\n\n" + usage_text();
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
