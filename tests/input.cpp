#include "input.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("keyboard mapping matches documented GBA layout") {
    gen3recomp::HostKeys keys;
    keys.a = true;
    keys.b = true;
    keys.start = true;
    keys.select = true;
    keys.up = true;
    keys.l = true;
    keys.r = true;

    const auto buttons = gen3recomp::map_keyboard(keys);
    REQUIRE(buttons.a);
    REQUIRE(buttons.b);
    REQUIRE(buttons.start);
    REQUIRE(buttons.select);
    REQUIRE(buttons.up);
    REQUIRE(buttons.l);
    REQUIRE(buttons.r);
    REQUIRE_FALSE(buttons.down);

    const auto keyinput = buttons.keyinput();
    REQUIRE((keyinput & (1u << 0)) == 0);
    REQUIRE((keyinput & (1u << 1)) == 0);
    REQUIRE((keyinput & (1u << 6)) == 0);
    REQUIRE((keyinput & (1u << 7)) != 0);
}

TEST_CASE("released keys stay active-high in KEYINPUT") {
    const gen3recomp::GbaButtons buttons;
    REQUIRE(buttons.keyinput() == 0x03FF);
}
