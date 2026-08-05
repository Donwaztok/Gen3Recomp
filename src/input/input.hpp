#pragma once

#include "platform.hpp"

#include <cstdint>

namespace gen3recomp {

struct GbaButtons {
    bool a = false;
    bool b = false;
    bool select = false;
    bool start = false;
    bool right = false;
    bool left = false;
    bool up = false;
    bool down = false;
    bool r = false;
    bool l = false;

    std::uint16_t keyinput() const;
};

GbaButtons map_keyboard(const HostKeys& keys);

}
