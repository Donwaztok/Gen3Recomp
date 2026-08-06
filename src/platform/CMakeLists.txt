add_library(gen3recomp_platform STATIC platform.cpp sdl3_dyn.cpp)
target_include_directories(gen3recomp_platform PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# Headers only from SDL3 — runtime entry points are dlopen'd (avoid SDL2 collision).
target_link_libraries(gen3recomp_platform PUBLIC gen3recomp_core)

# Prefer the headers-only imported target (SDL 3.2+). Linking SDL3::SDL3 would
# pull the shared library into the link line, which we intentionally avoid.
if(TARGET SDL3::Headers)
    target_link_libraries(gen3recomp_platform PUBLIC SDL3::Headers)
elseif(TARGET SDL3::SDL3)
    # Propagate include dirs via generator expression (raw get_target_property
    # drops $<INSTALL_INTERFACE:...> and breaks setup-sdl / prefix installs).
    target_include_directories(
        gen3recomp_platform PUBLIC
        $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>)
elseif(DEFINED SDL3_INCLUDE_DIRS)
    target_include_directories(gen3recomp_platform PUBLIC ${SDL3_INCLUDE_DIRS})
endif()

# setup-sdl / manual installs: SDL3_ROOT/include/SDL3/SDL.h
if(DEFINED ENV{SDL3_ROOT} AND EXISTS "$ENV{SDL3_ROOT}/include")
    target_include_directories(gen3recomp_platform PUBLIC "$ENV{SDL3_ROOT}/include")
endif()
if(SDL3_ROOT AND EXISTS "${SDL3_ROOT}/include")
    target_include_directories(gen3recomp_platform PUBLIC "${SDL3_ROOT}/include")
endif()

if(UNIX)
    target_link_libraries(gen3recomp_platform PUBLIC ${CMAKE_DL_LIBS})
endif()
