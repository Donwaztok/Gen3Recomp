#pragma once

#include <cstdint>
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
};

}
