#pragma once

#include <cstdint>

namespace gen3recomp {

struct HostKeys {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool a = false;
    bool b = false;
    bool start = false;
    bool select = false;
    bool l = false;
    bool r = false;
    bool quit = false;
};

class Platform {
public:
    bool init(const char* title, int window_width, int window_height);
    void shutdown();
    bool poll_quit();
    bool present_rgba32(const std::uint32_t* pixels, int width, int height);
    void delay_ms(std::uint32_t milliseconds);
    const HostKeys& keys() const;

private:
    struct Impl;
    Impl* impl_ = nullptr;
    HostKeys keys_{};
};

}
