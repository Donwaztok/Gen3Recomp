#pragma once

#include "game_definition.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gen3recomp {

class Catalog {
public:
    static Catalog builtin();

    void add(GameDefinition definition);
    std::optional<GameDefinition> find_by_sha1(std::string_view sha1) const;
    const std::vector<GameDefinition>& games() const { return games_; }

private:
    std::vector<GameDefinition> games_;
};

bool is_known_bios_sha1(std::string_view sha1);

}
