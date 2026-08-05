add_library(gen3recomp_platform STATIC platform.cpp)
target_include_directories(gen3recomp_platform PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(gen3recomp_platform PUBLIC gen3recomp_core SDL3::SDL3)
