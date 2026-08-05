#pragma once

#include "input.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace gen3recomp {

struct Frame {
    std::vector<std::uint32_t> pixels;
    int width = 0;
    int height = 0;
};

class SessionBackend {
public:
    virtual ~SessionBackend() = default;
    virtual bool start() = 0;
    virtual Frame step() = 0;
    virtual void stop() = 0;
    virtual bool owns_host_loop() const { return false; }
    virtual void set_input(const GbaButtons&) {}
    virtual void drain_audio(std::vector<std::int16_t>& out) { out.clear(); }
};

}
