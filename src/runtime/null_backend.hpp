#pragma once

#include "session_backend.hpp"

namespace gen3recomp {

class NullBackend final : public SessionBackend {
public:
    bool start() override;
    Frame step() override;
    void stop() override;
};

}
