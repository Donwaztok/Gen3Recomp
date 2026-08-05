add_executable(gen3recomp_tests smoke.cpp launcher.cpp)
target_link_libraries(gen3recomp_tests PRIVATE gen3recomp_app Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(gen3recomp_tests)
