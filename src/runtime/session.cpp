#include "session.hpp"

#include "audio.hpp"
#include "input.hpp"
#include "platform.hpp"
#include "session_backend.hpp"

namespace gen3recomp {

bool Session::start(SessionBackend& backend) {
    running_ = backend.start();
    return running_;
}

bool Session::tick(Platform& platform, SessionBackend& backend, AudioDevice* audio) {
    if (!running_) {
        return false;
    }
    if (platform.poll_quit()) {
        running_ = false;
        return false;
    }
    backend.set_input(map_keyboard(platform.keys()));
    const Frame frame = backend.step();
    if (frame.width <= 0 || frame.height <= 0 || frame.pixels.empty()) {
        running_ = false;
        return false;
    }
    if (!platform.present_rgba32(frame.pixels.data(), frame.width, frame.height)) {
        running_ = false;
        return false;
    }
    if (audio != nullptr) {
        std::vector<std::int16_t> samples;
        backend.drain_audio(samples);
        audio->queue(samples);
    }
    platform.delay_ms(16);
    return true;
}

void Session::stop(SessionBackend& backend) {
    backend.stop();
    running_ = false;
}

int Session::run(SessionBackend& backend) {
    if (!backend.owns_host_loop()) {
        return 1;
    }
    return start(backend) ? 0 : 1;
}

int Session::run(Platform& platform, SessionBackend& backend, AudioDevice* audio) {
    if (backend.owns_host_loop()) {
        return run(backend);
    }
    if (!start(backend)) {
        return 1;
    }
    while (tick(platform, backend, audio)) {
    }
    stop(backend);
    return 0;
}

}
