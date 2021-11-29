project(godzilla-test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

add_test(
    NAME godzilla-test
    COMMAND ${PROJECT_NAME}
)
if(GODZILLA_CODE_COVERAGE)
    set_tests_properties(
        godzilla-test
        PROPERTIES
            ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}.profraw
    )
endif()
