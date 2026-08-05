add_executable(gen3recomp_tests smoke.cpp launcher.cpp sha1.cpp identification.cpp video.cpp platform.cpp session.cpp)
target_link_libraries(gen3recomp_tests PRIVATE gen3recomp_app Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(gen3recomp_tests)

add_test(
    NAME runtime_has_no_gbarecomp_includes
    COMMAND "${CMAKE_COMMAND}" -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -P "${CMAKE_CURRENT_SOURCE_DIR}/check_runtime_isolation.cmake"
)
