#include "catalog.hpp"

#include <algorithm>
#include <array>
#include <cctype>

namespace gen3recomp {
namespace {

std::string normalize_sha1(std::string_view sha1) {
    std::string out;
    out.reserve(sha1.size());
    for (char ch : sha1) {
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return out;
}

}  // namespace

void Catalog::add(GameDefinition definition) {
    definition.sha1 = normalize_sha1(definition.sha1);
    games_.push_back(std::move(definition));
}

std::optional<GameDefinition> Catalog::find_by_sha1(std::string_view sha1) const {
    const auto needle = normalize_sha1(sha1);
    const auto it = std::find_if(games_.begin(), games_.end(), [&](const GameDefinition& game) {
        return game.sha1 == needle;
    });
    if (it == games_.end()) {
        return std::nullopt;
    }
    return *it;
}

Catalog Catalog::builtin() {
    Catalog catalog;
    // No-Intro / TASVideos verified dumps. Sources recorded in data/catalog_sources.md.
    catalog.add({
        "ruby-usa",
        "Pokémon Ruby",
        "USA",
        "f28b6ffc97847e94a6c21a63cacf633ee5c8df1e",
        "gen3",
        "flash1m",
        true,
    });
    catalog.add({
        "sapphire-usa",
        "Pokémon Sapphire",
        "USA",
        "3ccbbd45f8553c36463f13b938e833f652b793e4",
        "gen3",
        "flash1m",
        true,
    });
    catalog.add({
        "sapphire-usa-rev1",
        "Pokémon Sapphire",
        "USA",
        "4722efb8cd45772ca32555b98fd3b9719f8e60a9",
        "gen3",
        "flash1m",
        true,
    });
    catalog.add({
        "emerald-usa",
        "Pokémon Emerald",
        "USA",
        "f3ae088181bf583e55daf962a92bb46f4f1d07b7",
        "gen3",
        "flash1m",
        true,
    });
    return catalog;
}

bool is_known_bios_sha1(std::string_view sha1) {
    static constexpr std::array<std::string_view, 2> kKnown{
        "300c20df6731a33952ded8c436f7f186d25d3492",  // GBA / GBA SP / Micro / Game Boy Player
        "c11531d5261006810cdc954bd4bec0afe3187b35",  // DS / DS Lite / 3DS dump
    };
    const auto needle = normalize_sha1(sha1);
    return std::any_of(kKnown.begin(), kKnown.end(), [&](std::string_view known) { return known == needle; });
}

}
