#include "video.hpp"

namespace gen3recomp {

int window_size_for_scale(int logical_size, int scale) {
    if (logical_size <= 0 || scale <= 0) {
        return 0;
    }
    return logical_size * scale;
}

void fill_placeholder_frame(std::span<std::uint32_t> pixels, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::uint32_t red = static_cast<std::uint32_t>((x * 255) / (width - 1));
            const std::uint32_t green = static_cast<std::uint32_t>((y * 255) / (height - 1));
            const std::uint32_t blue = 64;
            pixels[static_cast<std::size_t>(y * width + x)] = (red << 16U) | (green << 8U) | blue;
        }
    }
}

}
