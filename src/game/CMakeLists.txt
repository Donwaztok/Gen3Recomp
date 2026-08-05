add_library(gen3recomp_game STATIC catalog.cpp)
target_include_directories(gen3recomp_game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(gen3recomp_game PUBLIC gen3recomp_core)
