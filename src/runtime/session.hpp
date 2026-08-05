#pragma once

namespace gen3recomp {

class AudioDevice;
class Platform;
class SessionBackend;

class Session {
public:
    bool start(SessionBackend& backend);
    bool tick(Platform& platform, SessionBackend& backend, AudioDevice* audio = nullptr);
    void stop(SessionBackend& backend);
    int run(SessionBackend& backend);
    int run(Platform& platform, SessionBackend& backend, AudioDevice* audio = nullptr);

private:
    bool running_ = false;
};

}
