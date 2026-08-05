set(roots
    "${SOURCE_DIR}/src/app"
    "${SOURCE_DIR}/src/runtime"
    "${SOURCE_DIR}/src/platform"
    "${SOURCE_DIR}/src/video"
    "${SOURCE_DIR}/src/audio"
    "${SOURCE_DIR}/src/input"
    "${SOURCE_DIR}/src/game"
    "${SOURCE_DIR}/src/core"
)
foreach(root IN LISTS roots)
    if(NOT EXISTS "${root}")
        continue()
    endif()
    file(GLOB_RECURSE sources "${root}/*")
    foreach(source IN LISTS sources)
        file(READ "${source}" contents)
        if(contents MATCHES "gbarecomp/" OR contents MATCHES "gba-recomp")
            message(FATAL_ERROR "${source} must not include gba-recomp")
        endif()
    endforeach()
endforeach()
