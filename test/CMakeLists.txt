function(add_tz_test)
    cmake_parse_arguments(
        ADD_TZ_TEST
        "GRAPHICAL"
        "NAME"
        "SOURCE_FILES;SHADER_SOURCES"
        ${ARGN}
        )

    add_executable(${ADD_TZ_TEST_NAME}
        ${ADD_TZ_TEST_SOURCE_FILES}
    )
    target_include_directories(${ADD_TZ_TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
    target_link_libraries(${ADD_TZ_TEST_NAME} PRIVATE topaz tz_test_harness)
    add_shader(
        TARGET ${ADD_TZ_TEST_NAME}
        SHADERS ${ADD_TZ_TEST_SHADER_SOURCES}
    )
    add_test(NAME ${ADD_TZ_TEST_NAME}
        COMMAND ${ADD_TZ_TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
    add_dependencies(tztest ${ADD_TZ_TEST_NAME})
    add_dependencies(tztest_nographics ${ADD_TZ_TEST_NAME})
    if(${ADD_TZ_TEST_GRAPHICAL})
        set_tests_properties(${ADD_TZ_TEST_NAME} PROPERTIES LABELS GRAPHICAL)
        message(STATUS "Found graphical test ${ADD_TZ_TEST_NAME}")
    else()
        message(STATUS "Found non-graphical test ${ADD_TZ_TEST_NAME}")
    endif()
endfunction()

add_custom_target(tztest COMMAND "${CMAKE_CTEST_COMMAND}" -V --output-on-failure -C Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

add_custom_target(tztest_nographics COMMAND "${CMAKE_CTEST_COMMAND}" -V --output-on-failure -LE GRAPHICAL -C Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

add_library(tz_test_harness INTERFACE)
target_include_directories(tz_test_harness INTERFACE ./)

add_subdirectory(core)
add_subdirectory(gl)