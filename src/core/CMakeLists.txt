add_library(gen3recomp_core INTERFACE)
target_include_directories(gen3recomp_core INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(gen3recomp_core INTERFACE GEN3RECOMP_VERSION="${PROJECT_VERSION}-dev")
