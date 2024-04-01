project(godzilla-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        -DGODZILLA_UNIT_TESTS_ROOT="${GODZILLA_UNIT_TESTS_ROOT}"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
    PRIVATE
        ${PETSC_INCLUDE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        yaml-cpp::yaml-cpp
        godzilla
        ${PETSC_LIBRARY}
        gtest_main
        gmock_main
)
if (GODZILLA_WITH_MPI)
    target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_CXX)
endif()

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
