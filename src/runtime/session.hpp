#pragma once

namespace gen3recomp {

class Platform;
class SessionBackend;

class Session {
public:
    bool start(SessionBackend& backend);
    bool tick(Platform& platform, SessionBackend& backend);
    void stop(SessionBackend& backend);
    int run(Platform& platform, SessionBackend& backend);

private:
    bool running_ = false;
};

}
