add_library(gen3recomp_input STATIC input.cpp)
target_include_directories(gen3recomp_input PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(gen3recomp_input PUBLIC gen3recomp_core gen3recomp_platform)
