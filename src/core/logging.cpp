#include "logging.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

void init_logging() {
    auto logger = spdlog::stderr_color_mt("gen3recomp");
    logger->set_level(spdlog::level::info);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::set_default_logger(std::move(logger));
}

void log_error(const char* message) {
    spdlog::error("{}", message);
}
