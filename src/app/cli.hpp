#pragma once

#include "exit_codes.hpp"
#include "launch_request.hpp"

#include <string>

namespace gen3recomp {

struct ParseResult {
    ExitCode code = ExitCode::Ok;
    LaunchRequest request;
    std::string message;
};

ParseResult parse_args(int argc, char** argv);
std::string usage_text();

}
