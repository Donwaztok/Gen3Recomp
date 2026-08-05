#pragma once

#include <cstdint>

namespace gen3recomp {

class Platform {
public:
    bool init(const char* title, int window_width, int window_height);
    void shutdown();
    bool poll_quit();
    bool present_rgba32(const std::uint32_t* pixels, int width, int height);
    void delay_ms(std::uint32_t milliseconds);

private:
    struct Impl;
    Impl* impl_ = nullptr;
};

}
