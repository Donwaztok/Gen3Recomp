add_executable(gen3recomp_tests smoke.cpp launcher.cpp launcher_aot.cpp sha1.cpp identification.cpp video.cpp platform.cpp session.cpp recomp_provider.cpp input.cpp audio.cpp saves.cpp)
target_link_libraries(gen3recomp_tests PRIVATE gen3recomp_app Catch2::Catch2WithMain)
if(TARGET gbarecomp_runtime)
    set(_gen3_gba_test_libs
        gen3recomp_recomp
        gbarecomp_runtime
        gbarecomp_armv4t
        gbarecomp_gba
        gbarecomp_debug)
    if(APPLE)
        target_link_libraries(gen3recomp_tests PRIVATE
            ${_gen3_gba_test_libs}
            ${_gen3_gba_test_libs}
            ${CMAKE_DL_LIBS})
    elseif(MSVC)
        target_link_libraries(gen3recomp_tests PRIVATE ${_gen3_gba_test_libs})
    else()
        target_link_libraries(gen3recomp_tests PRIVATE
            "-Wl,--start-group"
            ${_gen3_gba_test_libs}
            "-Wl,--end-group"
            ${CMAKE_DL_LIBS}
            pthread)
    endif()
endif()
if(TARGET gen3recomp_cart_artifact)
    get_target_property(_gen3_cart_loc gen3recomp_cart_artifact IMPORTED_LOCATION)
    get_filename_component(_gen3_cart_dir "${_gen3_cart_loc}" DIRECTORY)
    set_target_properties(gen3recomp_tests PROPERTIES
        BUILD_RPATH "${_gen3_cart_dir}"
        INSTALL_RPATH "${_gen3_cart_dir}")
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
