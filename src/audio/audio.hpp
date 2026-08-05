#pragma once

#include <cstdint>
#include <span>

namespace gen3recomp {

class AudioDevice {
public:
    bool init(int sample_rate = 32768);
    void shutdown();
    void queue(std::span<const std::int16_t> samples);
    bool available() const { return available_; }

private:
    struct Impl;
    Impl* impl_ = nullptr;
    bool available_ = false;
};

}
