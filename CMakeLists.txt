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
# libcart.so (user-data cart AOT) binds host ABI symbols such as g_cpu / bus_* /
# runtime_* against the main executable. Data relocs cannot be lazy; without
# ENABLE_EXPORTS those symbols stay out of the dynamic table and dlopen fails.
set_target_properties(gen3recomp PROPERTIES ENABLE_EXPORTS ON)
if(TARGET gbarecomp_runtime)
    set(_gen3_gba_libs
        gen3recomp_recomp
        gbarecomp_runtime
        gbarecomp_armv4t
        gbarecomp_gba
        gbarecomp_debug)
    if(APPLE)
        # Apple ld has no --start-group/--end-group; list twice to break cycles.
        target_link_libraries(gen3recomp PRIVATE
            ${_gen3_gba_libs}
            ${_gen3_gba_libs}
            ${CMAKE_DL_LIBS})
    elseif(MSVC)
        target_link_libraries(gen3recomp PRIVATE ${_gen3_gba_libs})
    else()
        target_link_libraries(gen3recomp PRIVATE
            "-Wl,--start-group"
            ${_gen3_gba_libs}
            "-Wl,--end-group"
            ${CMAKE_DL_LIBS}
            pthread)
    endif()
endif()
if(TARGET gen3recomp_cart_artifact)
    get_target_property(_gen3_cart_loc gen3recomp_cart_artifact IMPORTED_LOCATION)
    get_filename_component(_gen3_cart_dir "${_gen3_cart_loc}" DIRECTORY)
    set_target_properties(gen3recomp PROPERTIES
        BUILD_RPATH "${_gen3_cart_dir}"
        INSTALL_RPATH "${_gen3_cart_dir}")
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
