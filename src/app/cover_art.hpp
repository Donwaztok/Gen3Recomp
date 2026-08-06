#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace gen3recomp {

struct CoverUrls {
    std::string primary;
    std::string fallback;
};

struct CoverPixels {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> rgba;  // width*height*4
};

std::filesystem::path cover_cache_dir();
std::filesystem::path cover_cache_path(const std::string& game_id);
std::filesystem::path cover_local_override_path(const std::string& game_id);

CoverUrls cover_urls_for(const std::string& game_id);

// Resolve existing bytes: local override → cache. Does not hit the network.
std::optional<std::filesystem::path> find_cached_cover(const std::string& game_id);

// HTTPS fetch into cache (primary then fallback). Returns cache path on success.
std::optional<std::filesystem::path> fetch_cover_to_cache(
    const std::string& game_id,
    std::string& error);

// Decode PNG/JPEG file to RGBA. Empty on failure.
std::optional<CoverPixels> load_cover_pixels(const std::filesystem::path& path);

}  // namespace gen3recomp
