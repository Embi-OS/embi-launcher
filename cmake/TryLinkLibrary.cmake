# TryLinkLibrary cmake file

function(target_try_link_libraries PROJECT_NAME LIBRARY PREFIX)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(${PREFIX} ${LIBRARY})

    if(${PREFIX}_FOUND)
        message(STATUS "${LIBRARY} version ${${PREFIX}_VERSION} found. Building with ${PREFIX} support.")
        target_include_directories(${PROJECT_NAME} PRIVATE ${${PREFIX}_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${${PREFIX}_LIBRARIES})
        target_compile_definitions(${PROJECT_NAME} PRIVATE ${PREFIX}_FOUND)
    else()
        message(STATUS "${LIBRARY} not found. Building without ${PREFIX} support.")
    endif()

endfunction()

