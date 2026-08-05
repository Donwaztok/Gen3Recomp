cmake_minimum_required(VERSION 3.20)

project(gen3recomp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Dependencies.cmake)

add_subdirectory(src/core)
add_subdirectory(src/game)
add_subdirectory(src/platform)
add_subdirectory(src/video)
add_subdirectory(src/audio)
add_subdirectory(src/input)
add_subdirectory(src/runtime)
add_subdirectory(src/recomp)
add_subdirectory(src/app)

add_executable(gen3recomp src/main.cpp)
target_link_libraries(gen3recomp PRIVATE gen3recomp_app)
if(TARGET gbarecomp_runtime)
    target_link_libraries(gen3recomp PRIVATE
        "-Wl,--start-group"
        gen3recomp_recomp
        gbarecomp_runtime
        gbarecomp_armv4t
        gbarecomp_gba
        gbarecomp_debug
        "-Wl,--end-group"
        ${CMAKE_DL_LIBS}
        pthread)
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
