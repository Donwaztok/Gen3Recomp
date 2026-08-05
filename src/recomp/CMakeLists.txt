set(GEN3_ROM_GEN "${CMAKE_SOURCE_DIR}/generated/rom")
set(GEN3_RECOMP_SOURCES gba/gba_provider.cpp gba/gba_backend.cpp gba/cart_artifact.cpp)
set(GEN3_HAS_STATIC_CART OFF)
set(GEN3_CART_VIA_ARTIFACT OFF)

# Prefer a prebuilt cart shared library (one-time compile under user data) over
# compiling gitignored generated/rom sources into the host on every rebuild.
set(GEN3RECOMP_CART_ARTIFACT "" CACHE FILEPATH
    "Optional prebuilt libcart.so exporting kDispatchTable (user-data cart AOT)")

if(GEN3RECOMP_CART_ARTIFACT AND EXISTS "${GEN3RECOMP_CART_ARTIFACT}")
    add_library(gen3recomp_cart_artifact SHARED IMPORTED GLOBAL)
    set_target_properties(gen3recomp_cart_artifact PROPERTIES
        IMPORTED_LOCATION "${GEN3RECOMP_CART_ARTIFACT}")
    set(GEN3_HAS_STATIC_CART ON)
    set(GEN3_CART_VIA_ARTIFACT ON)
    message(STATUS "gen3recomp: linking cart artifact ${GEN3RECOMP_CART_ARTIFACT}")
elseif(EXISTS "${GEN3_ROM_GEN}/dispatch_table.cpp")
    file(GLOB GEN3_ROM_BODIES CONFIGURE_DEPENDS "${GEN3_ROM_GEN}/recompiled_*.cpp")
    if(GEN3_ROM_BODIES)
        list(APPEND GEN3_RECOMP_SOURCES
            "${GEN3_ROM_GEN}/dispatch_table.cpp"
            ${GEN3_ROM_BODIES})
        if(EXISTS "${GEN3_ROM_GEN}/symbol_map.cpp")
            list(APPEND GEN3_RECOMP_SOURCES "${GEN3_ROM_GEN}/symbol_map.cpp")
        endif()
        set(GEN3_HAS_STATIC_CART ON)
        list(LENGTH GEN3_ROM_BODIES GEN3_ROM_SHARD_COUNT)
        message(STATUS "gen3recomp: linking local cart recomp (${GEN3_ROM_SHARD_COUNT} shards)")
    endif()
endif()

if(NOT GEN3_HAS_STATIC_CART)
    list(APPEND GEN3_RECOMP_SOURCES gba/dispatch_stub.cpp)
    message(STATUS "gen3recomp: no cart artifact or generated/rom — empty cart dispatch (self-heal only)")
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
if(GEN3_CART_VIA_ARTIFACT)
    # PUBLIC so gen3recomp / tests DT_NEEDED the cart .so and resolve kDispatchTable.
    target_link_libraries(gen3recomp_recomp PUBLIC gen3recomp_cart_artifact)
endif()
if(GEN3_HAS_STATIC_CART)
    target_compile_definitions(gen3recomp_recomp PRIVATE GEN3RECOMP_HAS_STATIC_CART=1)
    if(GEN3_CART_VIA_ARTIFACT)
        target_compile_definitions(gen3recomp_recomp PRIVATE GEN3RECOMP_CART_VIA_ARTIFACT=1)
    endif()
    if(NOT GEN3_CART_VIA_ARTIFACT AND NOT MSVC)
        set(GEN3_ROM_HEAVY_SOURCES ${GEN3_ROM_BODIES} "${GEN3_ROM_GEN}/dispatch_table.cpp")
        if(EXISTS "${GEN3_ROM_GEN}/symbol_map.cpp")
            list(APPEND GEN3_ROM_HEAVY_SOURCES "${GEN3_ROM_GEN}/symbol_map.cpp")
        endif()
        set_source_files_properties(${GEN3_ROM_HEAVY_SOURCES}
            PROPERTIES COMPILE_OPTIONS "-w;-fno-var-tracking;-fno-var-tracking-assignments")
    endif()
endif()
