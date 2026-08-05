#include "app.hpp"

#include "version.hpp"

#include <cstdio>
#include <string_view>

int run_app(int argc, char** argv) {
    if (argc >= 2 && std::string_view{argv[1]} == "--version") {
        std::printf("gen3recomp %s\n", GEN3RECOMP_VERSION);
        return 0;
    }

    std::printf("gen3recomp %s\n", GEN3RECOMP_VERSION);
    return 0;
}
