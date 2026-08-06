#include "cart_build.hpp"

#include <cstdio>
#include <cstdlib>
#include <array>
#include <spdlog/spdlog.h>

namespace gen3recomp {
namespace {

bool looks_like_project_root(const std::filesystem::path& root) {
    std::error_code error;
    return std::filesystem::is_regular_file(root / "scripts" / "build_cart_artifact.sh", error);
}

}  // namespace

std::filesystem::path resolve_project_root() {
    if (const char* env = std::getenv("GEN3RECOMP_ROOT"); env != nullptr && *env != '\0') {
        const std::filesystem::path root{env};
        if (looks_like_project_root(root)) {
            return root;
        }
    }
#if defined(GEN3RECOMP_SOURCE_DIR)
    {
        const std::filesystem::path root{GEN3RECOMP_SOURCE_DIR};
        if (looks_like_project_root(root)) {
            return root;
        }
    }
#endif
    std::error_code error;
    const auto cwd = std::filesystem::current_path(error);
    if (!error && looks_like_project_root(cwd)) {
        return cwd;
    }
    if (!error && looks_like_project_root(cwd / "..")) {
        return std::filesystem::weakly_canonical(cwd / "..");
    }
    return cwd;
}

bool build_cart_artifact(
    const std::filesystem::path& rom_path,
    CartBuildLogFn on_log,
    std::string& error) {
    std::error_code exists_error;
    if (!std::filesystem::is_regular_file(rom_path, exists_error)) {
        error = "ROM path is not a regular file: " + rom_path.string();
        return false;
    }

    const auto root = resolve_project_root();
    const auto script = root / "scripts" / "build_cart_artifact.sh";
    if (!std::filesystem::is_regular_file(script, exists_error)) {
        error =
            "cannot find scripts/build_cart_artifact.sh (set GEN3RECOMP_ROOT to the project "
            "directory; looked in " +
            root.string() + ")";
        return false;
    }

    // Require a host C++ toolchain — surface a clear error before minutes of work.
    if (std::system("c++ --version >/dev/null 2>&1") != 0) {
        error =
            "no working C++ compiler (c++) on PATH — install a toolchain (e.g. gcc/clang) "
            "before Build";
        return false;
    }

    const std::string command =
        "bash " + script.string() + " " +
        // Quote the ROM path for the shell.
        "'" + rom_path.string() + "' 2>&1";

    spdlog::info("starting cart artifact build: {}", command);
    if (on_log) {
        on_log("Starting cart AOT build (may take several minutes)...");
    }

    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
        error = "failed to start build_cart_artifact.sh";
        return false;
    }

    std::array<char, 512> buffer{};
    std::string last_lines;
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        std::string line{buffer.data()};
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();
        }
        if (on_log) {
            on_log(line);
        }
        spdlog::info("[cart-build] {}", line);
        last_lines = line;
    }

    const int status = pclose(pipe);
    if (status != 0) {
        error = "cart artifact build failed";
        if (!last_lines.empty()) {
            error += ": " + last_lines;
        }
        error +=
            " — ensure gba_recompile is built (cmake --build build --target gba_recompile) "
            "and a C++ toolchain is installed";
        return false;
    }

    if (on_log) {
        on_log("Cart artifact build finished successfully.");
    }
    return true;
}

}  // namespace gen3recomp
