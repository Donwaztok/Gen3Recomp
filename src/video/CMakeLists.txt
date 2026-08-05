add_library(gen3recomp_video STATIC video.cpp)
target_include_directories(gen3recomp_video PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(gen3recomp_video PUBLIC gen3recomp_core)
