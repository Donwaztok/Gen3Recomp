#pragma once

#include <string>

namespace gen3recomp {

struct GameDefinition {
    std::string id;
    std::string display_name;
    std::string region;
    std::string sha1;
    std::string generation_family;
    std::string save_family;
    bool has_rtc = false;
};

}
