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

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")

find_package(SDL3 REQUIRED CONFIG)

option(GEN3RECOMP_FETCH_GBARECOMP "Clone the pinned gba-recomp sources into third_party/" OFF)
if(GEN3RECOMP_FETCH_GBARECOMP)
    FetchContent_Declare(
        gbarecomp_src
        GIT_REPOSITORY https://github.com/mstan/gbarecomp.git
        GIT_TAG 2952aff2bb42f49de5903acf22af8fea3e2e3dee
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/gbarecomp"
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(gbarecomp_src)
    if(NOT gbarecomp_src_POPULATED)
        FetchContent_Populate(gbarecomp_src)
    endif()
endif()
