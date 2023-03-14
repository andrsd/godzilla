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

if (GODZILLA_WITH_MPI)

add_test(
    NAME par-godzilla-test
    COMMAND mpirun -np 4 $<TARGET_FILE:par-godzilla-test>
)
set_property(TEST par-godzilla-test PROPERTY PROCESSORS 4)
set_property(TEST par-godzilla-test PROPERTY PROCESSOR_AFFINITY TRUE)

if(GODZILLA_CODE_COVERAGE)
    set_tests_properties(
        par-godzilla-test
        PROPERTIES
            ENVIRONMENT LLVM_PROFILE_FILE=par-${PROJECT_NAME}.profraw
    )
endif()

endif()
