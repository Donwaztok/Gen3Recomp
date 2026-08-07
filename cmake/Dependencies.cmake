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

# Host-required API not yet in the pinned upstream commit (cart dlopen dispatch
# override + MSVC clz + optional heal warm). Applied idempotently so CI clean
# checkouts and local dirty trees both work.
set(_GEN3_GBARECOMP_PATCH
    "${CMAKE_SOURCE_DIR}/third_party/patches/gbarecomp-gen3-host.patch")
if(EXISTS "${_GEN3_GBARECOMP_PATCH}")
    find_package(Git QUIET)
    if(Git_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" -C "${GBARECOMP_DIR}" apply --reverse --check
                    "${_GEN3_GBARECOMP_PATCH}"
            RESULT_VARIABLE _gen3_patch_already
            OUTPUT_QUIET
            ERROR_QUIET)
        if(NOT _gen3_patch_already EQUAL 0)
            execute_process(
                COMMAND "${GIT_EXECUTABLE}" -C "${GBARECOMP_DIR}" apply
                        "${_GEN3_GBARECOMP_PATCH}"
                RESULT_VARIABLE _gen3_patch_apply
                OUTPUT_VARIABLE _gen3_patch_out
                ERROR_VARIABLE _gen3_patch_err)
            if(NOT _gen3_patch_apply EQUAL 0)
                message(FATAL_ERROR
                    "Failed to apply ${_GEN3_GBARECOMP_PATCH} to gba-recomp submodule.\n"
                    "${_gen3_patch_out}${_gen3_patch_err}")
            endif()
            message(STATUS "gen3recomp: applied gbarecomp-gen3-host.patch")
        else()
            message(STATUS "gen3recomp: gbarecomp-gen3-host.patch already applied")
        endif()
    else()
        message(WARNING "gen3recomp: git not found; cannot apply gbarecomp-gen3-host.patch")
    endif()
endif()

set(GBARECOMP_ENABLE_MODS OFF CACHE BOOL "" FORCE)
set(GBARECOMP_BUILD_ORACLE OFF CACHE BOOL "" FORCE)

# gba-recomp HostWindow needs SDL2 (real SDL2 or sdl2-compat). Without it the
# runtime stubs the window and silently runs one headless frame then exits —
# which is what broke GitHub Release player packages. Resolve into cache before
# add_subdirectory so gbarecomp's find_path/find_library reuse these values.
if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY)
    find_package(SDL2 QUIET CONFIG)
    if(TARGET SDL2::SDL2)
        get_target_property(_gen3_sdl2_inc SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
        if(_gen3_sdl2_inc)
            # Prefer .../include/SDL2 when the target exposes the parent include/.
            foreach(_inc IN LISTS _gen3_sdl2_inc)
                if(EXISTS "${_inc}/SDL.h")
                    set(SDL2_INCLUDE_DIR "${_inc}" CACHE PATH "SDL2 headers" FORCE)
                    break()
                elseif(EXISTS "${_inc}/SDL2/SDL.h")
                    set(SDL2_INCLUDE_DIR "${_inc}/SDL2" CACHE PATH "SDL2 headers" FORCE)
                    break()
                endif()
            endforeach()
        endif()
        # MSVC must link the import library (.lib), not the runtime DLL.
        set(_gen3_sdl2_loc "")
        foreach(_prop IMPORTED_IMPLIB_RELEASE IMPORTED_IMPLIB IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION)
            get_target_property(_cand SDL2::SDL2 ${_prop})
            if(_cand AND NOT _cand STREQUAL "_cand-NOTFOUND")
                set(_gen3_sdl2_loc "${_cand}")
                break()
            endif()
        endforeach()
        if(_gen3_sdl2_loc)
            set(SDL2_LIBRARY "${_gen3_sdl2_loc}" CACHE FILEPATH "SDL2 library" FORCE)
        endif()
    endif()
endif()
if(NOT SDL2_INCLUDE_DIR)
    find_path(SDL2_INCLUDE_DIR NAMES SDL.h PATH_SUFFIXES SDL2
        HINTS
            ENV SDL2_ROOT
            ENV CMAKE_PREFIX_PATH
            ${CMAKE_PREFIX_PATH}
            ${SDL2_ROOT}
            ${SDL3_ROOT}
        PATHS
            /usr/include
            /usr/local/include
            /opt/homebrew/include
    )
endif()
if(NOT SDL2_LIBRARY)
    # Prefer import libs on Windows (PATH_SUFFIXES lib before bin).
    find_library(SDL2_LIBRARY NAMES SDL2 SDL2-2.0
        HINTS
            ENV SDL2_ROOT
            ENV CMAKE_PREFIX_PATH
            ${CMAKE_PREFIX_PATH}
            ${SDL2_ROOT}
            ${SDL3_ROOT}
        PATH_SUFFIXES lib lib64
    )
endif()
# setup-sdl / CONFIG packages sometimes expose IMPORTED_LOCATION as the .dll;
# MSVC then dies with LNK1107. Rewrite to the sibling import library.
if(SDL2_LIBRARY MATCHES "\\.[Dd][Ll][Ll]$")
    get_filename_component(_gen3_sdl2_bin "${SDL2_LIBRARY}" DIRECTORY)
    get_filename_component(_gen3_sdl2_root "${_gen3_sdl2_bin}" DIRECTORY)
    set(_gen3_sdl2_implib "")
    foreach(_cand
            "${_gen3_sdl2_root}/lib/SDL2.lib"
            "${_gen3_sdl2_root}/lib/SDL2.dll.a"
            "${_gen3_sdl2_bin}/SDL2.lib")
        if(EXISTS "${_cand}")
            set(_gen3_sdl2_implib "${_cand}")
            break()
        endif()
    endforeach()
    if(_gen3_sdl2_implib)
        message(STATUS "gen3recomp: rewriting SDL2.dll link item -> ${_gen3_sdl2_implib}")
        set(SDL2_LIBRARY "${_gen3_sdl2_implib}" CACHE FILEPATH "SDL2 library" FORCE)
    else()
        message(FATAL_ERROR
            "SDL2 resolved to a DLL unsuitable for MSVC linking:\n"
            "  ${SDL2_LIBRARY}\n"
            "Expected an import library (SDL2.lib) under lib/ next to that prefix.")
    endif()
endif()
if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY)
    message(FATAL_ERROR
        "SDL2 (or sdl2-compat) is required for the player window.\n"
        "  SDL2_INCLUDE_DIR=${SDL2_INCLUDE_DIR}\n"
        "  SDL2_LIBRARY=${SDL2_LIBRARY}\n"
        "Install SDL2 / sdl2-compat and pass -DCMAKE_PREFIX_PATH=... (CI: setup-sdl).\n"
        "Without SDL2, gba-recomp stubs HostWindow and exits after one headless frame.")
endif()
message(STATUS "gen3recomp: SDL2 for gba-recomp HostWindow — inc=${SDL2_INCLUDE_DIR} lib=${SDL2_LIBRARY}")

add_subdirectory("${GBARECOMP_DIR}" "${CMAKE_BINARY_DIR}/_gbarecomp" EXCLUDE_FROM_ALL)
# Host packaging/CI builds gba_recompile explicitly; clear EXCLUDE_FROM_ALL so the
# Visual Studio generator emits a reachable .vcxproj for --target gba_recompile.
if(TARGET gba_recompile)
    set_target_properties(gba_recompile PROPERTIES EXCLUDE_FROM_ALL FALSE)
endif()
get_target_property(_gen3_gba_defs gbarecomp_runtime INTERFACE_COMPILE_DEFINITIONS)
set(_gen3_have_sdl2 FALSE)
if(_gen3_gba_defs)
    foreach(_def IN LISTS _gen3_gba_defs)
        if(_def STREQUAL "GBARECOMP_HAVE_SDL2")
            set(_gen3_have_sdl2 TRUE)
        endif()
    endforeach()
endif()
if(NOT _gen3_have_sdl2)
    message(FATAL_ERROR
        "gbarecomp_runtime built without GBARECOMP_HAVE_SDL2 — HostWindow is stubbed.\n"
        "Fix SDL2 discovery (see gen3recomp SDL2 status above) and reconfigure.")
endif()
get_directory_property(_gbarecomp_tests DIRECTORY "${GBARECOMP_DIR}" TESTS)
foreach(_gbarecomp_test IN LISTS _gbarecomp_tests)
    if(TEST "${_gbarecomp_test}")
        set_tests_properties("${_gbarecomp_test}" PROPERTIES DISABLED TRUE)
    endif()
endforeach()
