#include "cover_art.hpp"

#include "user_data.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <cstdio>
#include <fstream>
#include <mutex>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_THREAD_LOCALS
#include "stb_image.h"

namespace gen3recomp {
namespace {

std::mutex g_cover_mu;

struct BuiltinCover {
    const char* id;
    const char* primary;
    const char* fallback;
};

// Mirrors data/covers.toml (URLs only — no image payloads in the binary beyond strings).
constexpr std::array<BuiltinCover, 4> kCovers{{
    {"ruby-usa",
     "https://upload.wikimedia.org/wikipedia/en/b/be/PokemonRubySapphireBox.jpg",
     "https://www.thecoverproject.net/images/covers/gbapkmnrubyusa.jpg"},
    {"sapphire-usa",
     "https://upload.wikimedia.org/wikipedia/en/b/be/PokemonRubySapphireBox.jpg",
     "https://www.thecoverproject.net/images/covers/gbapkmnsapphireusa.jpg"},
    {"sapphire-usa-rev1",
     "https://upload.wikimedia.org/wikipedia/en/b/be/PokemonRubySapphireBox.jpg",
     "https://www.thecoverproject.net/images/covers/gbapkmnsapphireusa.jpg"},
    {"emerald-usa",
     "https://upload.wikimedia.org/wikipedia/pt/7/72/Pok%C3%A9mon_Emerald_cover.png",
     "https://www.thecoverproject.net/images/covers/gbapkmnemeraldusa.jpg"},
}};

bool download_url(const std::string& url, const std::filesystem::path& dest, std::string& error) {
    std::error_code ec;
    std::filesystem::create_directories(dest.parent_path(), ec);
    const auto tmp = dest.string() + ".partial";
#if defined(_WIN32)
    const std::string cmd =
        "curl.exe -fsSL --max-time 30 -A \"gen3recomp/0.1\" -o \"" + tmp + "\" \"" + url + "\"";
#else
    const std::string cmd =
        "curl -fsSL --max-time 30 -A \"gen3recomp/0.1\" -o '" + tmp + "' '" + url + "'";
#endif
    spdlog::info("cover fetch: {}", url);
    const int rc = std::system(cmd.c_str());
    if (rc != 0) {
        std::filesystem::remove(tmp, ec);
        error = "curl failed for " + url;
        return false;
    }
    // Reject tiny/error HTML bodies.
    const auto size = std::filesystem::file_size(tmp, ec);
    if (ec || size < 1024) {
        std::filesystem::remove(tmp, ec);
        error = "download too small or missing: " + url;
        return false;
    }
    std::filesystem::rename(tmp, dest, ec);
    if (ec) {
        error = "failed to move cover into cache: " + ec.message();
        return false;
    }
    return true;
}

}  // namespace

std::filesystem::path cover_cache_dir() {
    return user_data_dir() / "covers";
}

std::filesystem::path cover_cache_path(const std::string& game_id) {
    return cover_cache_dir() / (game_id + ".img");
}

std::filesystem::path cover_local_override_path(const std::string& game_id) {
    return std::filesystem::path{"roms"} / "covers" / (game_id + ".png");
}

CoverUrls cover_urls_for(const std::string& game_id) {
    for (const auto& row : kCovers) {
        if (game_id == row.id) {
            return CoverUrls{row.primary, row.fallback};
        }
    }
    return {};
}

std::optional<std::filesystem::path> find_cached_cover(const std::string& game_id) {
    std::error_code ec;
    const auto override_png = cover_local_override_path(game_id);
    if (std::filesystem::is_regular_file(override_png, ec)) {
        return override_png;
    }
    const auto override_jpg = std::filesystem::path{"roms"} / "covers" / (game_id + ".jpg");
    if (std::filesystem::is_regular_file(override_jpg, ec)) {
        return override_jpg;
    }
    const auto cached = cover_cache_path(game_id);
    if (std::filesystem::is_regular_file(cached, ec)) {
        return cached;
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> fetch_cover_to_cache(
    const std::string& game_id,
    std::string& error) {
    std::lock_guard lock{g_cover_mu};
    if (auto existing = find_cached_cover(game_id); existing.has_value()) {
        // Prefer not to overwrite local overrides; cache hit returns existing.
        if (existing->string().find("roms/covers") != std::string::npos ||
            existing->string().find("roms\\covers") != std::string::npos) {
            return existing;
        }
        return existing;
    }

    const auto urls = cover_urls_for(game_id);
    if (urls.primary.empty() && urls.fallback.empty()) {
        error = "no curated cover URL for " + game_id;
        return std::nullopt;
    }

    const auto dest = cover_cache_path(game_id);
    std::string last_error;
    if (!urls.primary.empty() && download_url(urls.primary, dest, last_error)) {
        return dest;
    }
    if (!urls.fallback.empty() && download_url(urls.fallback, dest, last_error)) {
        return dest;
    }
    error = last_error.empty() ? "cover fetch failed" : last_error;
    return std::nullopt;
}

std::optional<CoverPixels> load_cover_pixels(const std::filesystem::path& path) {
    int w = 0;
    int h = 0;
    int comp = 0;
    unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &comp, 4);
    if (data == nullptr || w <= 0 || h <= 0) {
        if (data != nullptr) {
            stbi_image_free(data);
        }
        return std::nullopt;
    }
    CoverPixels out;
    out.width = w;
    out.height = h;
    out.rgba.assign(data, data + static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 4u);
    stbi_image_free(data);
    return out;
}

}  // namespace gen3recomp
