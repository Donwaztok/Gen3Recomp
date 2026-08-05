#include "input.hpp"

namespace gen3recomp {

std::uint16_t GbaButtons::keyinput() const {
    std::uint16_t value = 0x03FF;
    const auto clear_bit = [&](int bit, bool pressed) {
        if (pressed) {
            value = static_cast<std::uint16_t>(value & ~(1u << bit));
        }
    };
    clear_bit(0, a);
    clear_bit(1, b);
    clear_bit(2, select);
    clear_bit(3, start);
    clear_bit(4, right);
    clear_bit(5, left);
    clear_bit(6, up);
    clear_bit(7, down);
    clear_bit(8, r);
    clear_bit(9, l);
    return value;
}

GbaButtons map_keyboard(const HostKeys& keys) {
    GbaButtons buttons;
    buttons.a = keys.a;
    buttons.b = keys.b;
    buttons.select = keys.select;
    buttons.start = keys.start;
    buttons.right = keys.right;
    buttons.left = keys.left;
    buttons.up = keys.up;
    buttons.down = keys.down;
    buttons.r = keys.r;
    buttons.l = keys.l;
    return buttons;
}

}
