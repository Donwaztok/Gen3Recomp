add_library(gen3recomp_app STATIC app.cpp cli.cpp)
target_include_directories(gen3recomp_app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(gen3recomp_app PUBLIC gen3recomp_core gen3recomp_game gen3recomp_platform gen3recomp_video gen3recomp_runtime)
