include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
    GIT_SHALLOW TRUE
)

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(fmt spdlog Catch2)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()
set(GEN3RECOMP_STB_INCLUDE_DIR "${stb_SOURCE_DIR}" CACHE INTERNAL "")

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")

find_package(SDL3 REQUIRED CONFIG)
if(TARGET SDL3::Headers)
    message(STATUS "gen3recomp: SDL3::Headers available")
elseif(TARGET SDL3::SDL3)
    message(STATUS "gen3recomp: SDL3::SDL3 available (headers via INTERFACE)")
else()
    message(STATUS "gen3recomp: SDL3 found without Headers/SDL3 targets — relying on SDL3_ROOT/include")
endif()

set(GBARECOMP_DIR "${CMAKE_SOURCE_DIR}/third_party/gbarecomp")
if(NOT EXISTS "${GBARECOMP_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "gba-recomp submodule is missing at third_party/gbarecomp.\n"
        "Run: git submodule update --init --recursive")
endif()

set(GBARECOMP_ENABLE_MODS OFF CACHE BOOL "" FORCE)
set(GBARECOMP_BUILD_ORACLE OFF CACHE BOOL "" FORCE)
add_subdirectory("${GBARECOMP_DIR}" "${CMAKE_BINARY_DIR}/_gbarecomp" EXCLUDE_FROM_ALL)
get_directory_property(_gbarecomp_tests DIRECTORY "${GBARECOMP_DIR}" TESTS)
foreach(_gbarecomp_test IN LISTS _gbarecomp_tests)
    if(TEST "${_gbarecomp_test}")
        set_tests_properties("${_gbarecomp_test}" PROPERTIES DISABLED TRUE)
    endif()
endforeach()
