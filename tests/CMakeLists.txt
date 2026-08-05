add_executable(gen3recomp_tests smoke.cpp launcher.cpp sha1.cpp identification.cpp video.cpp platform.cpp session.cpp recomp_provider.cpp input.cpp audio.cpp saves.cpp)
target_link_libraries(gen3recomp_tests PRIVATE gen3recomp_app Catch2::Catch2WithMain)
if(TARGET gbarecomp_runtime)
    target_link_libraries(gen3recomp_tests PRIVATE
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

include(Catch)
catch_discover_tests(gen3recomp_tests)

add_test(
    NAME runtime_has_no_gbarecomp_includes
    COMMAND "${CMAKE_COMMAND}" -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/check_runtime_isolation.cmake"
)

add_test(
    NAME host_modules_do_not_include_gbarecomp
    COMMAND "${CMAKE_COMMAND}" -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/check_adapter_isolation.cmake"
)
