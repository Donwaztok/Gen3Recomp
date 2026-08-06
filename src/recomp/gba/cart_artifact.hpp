#pragma once

#include <string>

namespace gen3recomp {

enum class CartCoverageKind {
    LinkedArtifact,
    LinkedGeneratedCorpus,
    RuntimeLoaded,
    UserDataArtifactPresent,
    HealOnly,
};

bool cart_artifact_ready(const std::string& rom_sha1);
CartCoverageKind detect_cart_coverage(const std::string& rom_sha1);
const char* cart_coverage_label(CartCoverageKind kind);
void log_cart_coverage(const std::string& rom_sha1);

void mark_cart_artifact_runtime_loaded(const std::string& rom_sha1);
void clear_cart_artifact_runtime_loaded();

bool try_activate_cart_artifact(const std::string& rom_sha1, std::string& error);
void clear_cart_artifact_activation();

}  // namespace gen3recomp
