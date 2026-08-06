#include "cart_artifact.hpp"

#include "user_data.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <mutex>
#include <string>

namespace gen3recomp {
namespace {

std::mutex g_coverage_mu;
std::string g_runtime_loaded_sha1;

}  // namespace

bool cart_artifact_ready(const std::string& rom_sha1) {
    if (rom_sha1.empty()) {
        return false;
    }
    std::error_code error;
    return std::filesystem::is_regular_file(cart_artifact_path(rom_sha1), error);
}

void mark_cart_artifact_runtime_loaded(const std::string& rom_sha1) {
    std::lock_guard lock{g_coverage_mu};
    g_runtime_loaded_sha1 = rom_sha1;
}

void clear_cart_artifact_runtime_loaded() {
    std::lock_guard lock{g_coverage_mu};
    g_runtime_loaded_sha1.clear();
}

CartCoverageKind detect_cart_coverage(const std::string& rom_sha1) {
#if defined(GEN3RECOMP_CART_VIA_ARTIFACT)
    (void)rom_sha1;
    return CartCoverageKind::LinkedArtifact;
#elif defined(GEN3RECOMP_HAS_STATIC_CART)
    (void)rom_sha1;
    return CartCoverageKind::LinkedGeneratedCorpus;
#else
    {
        std::lock_guard lock{g_coverage_mu};
        if (!g_runtime_loaded_sha1.empty() && g_runtime_loaded_sha1 == rom_sha1) {
            return CartCoverageKind::RuntimeLoaded;
        }
    }
    if (cart_artifact_ready(rom_sha1)) {
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
        case CartCoverageKind::RuntimeLoaded:
            return "runtime-loaded-artifact";
        case CartCoverageKind::UserDataArtifactPresent:
            return "user-data-artifact-ready";
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
            "Build once from the launcher or: ./scripts/build_cart_artifact.sh <rom.gba>");
    } else if (kind == CartCoverageKind::UserDataArtifactPresent) {
        spdlog::info(
            "cart artifact present; will activate via dlopen at session start ({})",
            artifact.string());
    }
}

}  // namespace gen3recomp
