#include "null_backend.hpp"

#include "video.hpp"

namespace gen3recomp {

bool NullBackend::start() {
    return true;
}

Frame NullBackend::step() {
    Frame frame;
    frame.width = kGbaWidth;
    frame.height = kGbaHeight;
    frame.pixels.resize(static_cast<std::size_t>(kGbaWidth * kGbaHeight));
    fill_placeholder_frame(frame.pixels, kGbaWidth, kGbaHeight);
    return frame;
}

void NullBackend::stop() {}

}
