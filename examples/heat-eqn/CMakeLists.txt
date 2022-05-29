project(heat-eqn)

add_subdirectory(src)

if (GODZILLA_BUILD_TESTS)
    add_subdirectory(test)
endif()
