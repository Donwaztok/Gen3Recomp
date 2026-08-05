#include "session.hpp"

#include "platform.hpp"
#include "session_backend.hpp"

namespace gen3recomp {

bool Session::start(SessionBackend& backend) {
    running_ = backend.start();
    return running_;
}

bool Session::tick(Platform& platform, SessionBackend& backend) {
    if (!running_) {
        return false;
    }
    if (platform.poll_quit()) {
        running_ = false;
        return false;
    }
    const Frame frame = backend.step();
    if (frame.width <= 0 || frame.height <= 0 || frame.pixels.empty()) {
        running_ = false;
        return false;
    }
    if (!platform.present_rgba32(frame.pixels.data(), frame.width, frame.height)) {
        running_ = false;
        return false;
    }
    platform.delay_ms(16);
    return true;
}

void Session::stop(SessionBackend& backend) {
    backend.stop();
    running_ = false;
}

int Session::run(Platform& platform, SessionBackend& backend) {
    if (!start(backend)) {
        return 1;
    }
    while (tick(platform, backend)) {
    }
    stop(backend);
    return 0;
}

}
