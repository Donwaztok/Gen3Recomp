#pragma once

#include <string>

namespace gen3recomp {

enum class CartCoverageKind {
    LinkedArtifact,
    LinkedGeneratedCorpus,
    UserDataArtifactPresent,
    HealOnly,
};

CartCoverageKind detect_cart_coverage(const std::string& rom_sha1);
const char* cart_coverage_label(CartCoverageKind kind);
void log_cart_coverage(const std::string& rom_sha1);

}  // namespace gen3recomp
