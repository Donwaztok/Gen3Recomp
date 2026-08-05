#include "app.hpp"

#include "logging.hpp"
#include "version.hpp"

#include <cstdio>
#include <spdlog/spdlog.h>
#include <string_view>

int run_app(int argc, char** argv) {
    init_logging();

    if (argc >= 2 && std::string_view{argv[1]} == "--version") {
        std::printf("gen3recomp %s\n", GEN3RECOMP_VERSION);
        return 0;
    }

    spdlog::info("gen3recomp {}", GEN3RECOMP_VERSION);
    std::printf("gen3recomp %s\n", GEN3RECOMP_VERSION);
    return 0;
}
