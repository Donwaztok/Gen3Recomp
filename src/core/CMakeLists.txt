add_library(gen3recomp_core STATIC logging.cpp sha1.cpp file_bytes.cpp user_data.cpp)
target_include_directories(gen3recomp_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(gen3recomp_core PUBLIC GEN3RECOMP_VERSION="${PROJECT_VERSION}-dev")
target_link_libraries(gen3recomp_core PUBLIC fmt::fmt spdlog::spdlog)
