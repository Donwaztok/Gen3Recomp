#include "cart_artifact.hpp"

#include "user_data.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>

namespace gen3recomp {

// Cart AOT consumption (adapter-private; not a plugin ABI — D6 / D-AOT-2):
//
// 1) Prefer CMake -DGEN3RECOMP_CART_ARTIFACT=<user-data libcart.so>
//    Built once by scripts/build_cart_artifact.sh. Host rebuilds only relink.
// 2) Else compile gitignored generated/rom/*.cpp into gen3recomp_recomp (dev).
// 3) Else empty dispatch_stub + runtime self-heal (slow cold ROM PCs).
//
// Upstream kDispatchTable is a link-time symbol (runtime_arm.cpp). A dedicated
// shared library exporting that table is the workable shape without forking
// gba-recomp registration APIs. Heal-cache DLLs remain for IWRAM overlays only.

CartCoverageKind detect_cart_coverage(const std::string& rom_sha1) {
#if defined(GEN3RECOMP_CART_VIA_ARTIFACT)
    (void)rom_sha1;
    return CartCoverageKind::LinkedArtifact;
#elif defined(GEN3RECOMP_HAS_STATIC_CART)
    (void)rom_sha1;
    return CartCoverageKind::LinkedGeneratedCorpus;
#else
    const auto artifact = cart_artifact_path(rom_sha1);
    std::error_code error;
    if (!rom_sha1.empty() && std::filesystem::is_regular_file(artifact, error)) {
        return CartCoverageKind::UserDataArtifactPresent;
    }
    return CartCoverageKind::HealOnly;
#endif
}

const char* cart_coverage_label(CartCoverageKind kind) {
    switch (kind) {
        case CartCoverageKind::LinkedArtifact:
            return "linked-artifact";
        case CartCoverageKind::LinkedGeneratedCorpus:
            return "linked-generated-rom";
        case CartCoverageKind::UserDataArtifactPresent:
            return "user-data-artifact-unlinked";
        case CartCoverageKind::HealOnly:
            return "heal-only";
    }
    return "unknown";
}

void log_cart_coverage(const std::string& rom_sha1) {
    const auto kind = detect_cart_coverage(rom_sha1);
    const auto artifact = cart_artifact_path(rom_sha1);
    spdlog::info(
        "cart coverage={} artifact_path={} (IWRAM overlays still use recomp_cache heal)",
        cart_coverage_label(kind),
        artifact.string());
    if (kind == CartCoverageKind::HealOnly) {
        spdlog::warn(
            "static cart AOT missing — ROM PCs self-heal on first visit (slow). "
            "Build once: ./scripts/build_cart_artifact.sh <rom.gba> then "
            "cmake -DGEN3RECOMP_CART_ARTIFACT={} -S . -B build && cmake --build build --target gen3recomp",
            artifact.string());
    } else if (kind == CartCoverageKind::UserDataArtifactPresent) {
        spdlog::warn(
            "cart artifact exists but this binary was not linked against it. "
            "Reconfigure with -DGEN3RECOMP_CART_ARTIFACT={}",
            artifact.string());
    }
}

}  // namespace gen3recomp
