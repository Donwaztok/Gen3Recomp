set(GEN3_ROM_GEN "${CMAKE_SOURCE_DIR}/generated/rom")
set(GEN3_RECOMP_SOURCES gba/gba_provider.cpp gba/gba_backend.cpp)
set(GEN3_HAS_STATIC_CART OFF)

file(GLOB GEN3_ROM_BODIES CONFIGURE_DEPENDS "${GEN3_ROM_GEN}/recompiled_*.cpp")
if(EXISTS "${GEN3_ROM_GEN}/dispatch_table.cpp" AND GEN3_ROM_BODIES)
    list(APPEND GEN3_RECOMP_SOURCES
        "${GEN3_ROM_GEN}/dispatch_table.cpp"
        ${GEN3_ROM_BODIES})
    if(EXISTS "${GEN3_ROM_GEN}/symbol_map.cpp")
        list(APPEND GEN3_RECOMP_SOURCES "${GEN3_ROM_GEN}/symbol_map.cpp")
    endif()
    set(GEN3_HAS_STATIC_CART ON)
    list(LENGTH GEN3_ROM_BODIES GEN3_ROM_SHARD_COUNT)
    message(STATUS "gen3recomp: linking local cart recomp (${GEN3_ROM_SHARD_COUNT} shards)")
else()
    list(APPEND GEN3_RECOMP_SOURCES gba/dispatch_stub.cpp)
    message(STATUS "gen3recomp: no generated/rom corpus — empty cart dispatch (self-heal only)")
endif()

add_library(gen3recomp_recomp STATIC ${GEN3_RECOMP_SOURCES})
target_include_directories(gen3recomp_recomp PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/gba")
target_link_libraries(gen3recomp_recomp PUBLIC gen3recomp_core gen3recomp_game gen3recomp_runtime)
if(TARGET gbarecomp_runtime)
    target_link_libraries(gen3recomp_recomp PRIVATE gbarecomp_runtime)
    target_compile_definitions(gen3recomp_recomp PRIVATE GEN3RECOMP_HAS_GBARECOMP=1)
    target_include_directories(gen3recomp_recomp PRIVATE
        "${gbarecomp_SOURCE_DIR}/src/armv4t"
        "${gbarecomp_SOURCE_DIR}/src/runtime"
        "${GEN3_ROM_GEN}")
endif()
if(GEN3_HAS_STATIC_CART)
    target_compile_definitions(gen3recomp_recomp PRIVATE GEN3RECOMP_HAS_STATIC_CART=1)
    if(NOT MSVC)
        set(GEN3_ROM_HEAVY_SOURCES ${GEN3_ROM_BODIES} "${GEN3_ROM_GEN}/dispatch_table.cpp")
        if(EXISTS "${GEN3_ROM_GEN}/symbol_map.cpp")
            list(APPEND GEN3_ROM_HEAVY_SOURCES "${GEN3_ROM_GEN}/symbol_map.cpp")
        endif()
        set_source_files_properties(${GEN3_ROM_HEAVY_SOURCES}
            PROPERTIES COMPILE_OPTIONS "-w;-fno-var-tracking;-fno-var-tracking-assignments")
    endif()
endif()
