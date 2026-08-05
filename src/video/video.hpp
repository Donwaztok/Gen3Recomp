#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace gen3recomp {

constexpr int kGbaWidth = 240;
constexpr int kGbaHeight = 160;

int window_size_for_scale(int logical_size, int scale);
void fill_placeholder_frame(std::span<std::uint32_t> pixels, int width, int height);

}
