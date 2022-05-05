project(poisson)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(POISSON_BIN ${PROJECT_NAME})

add_executable(
    ${POISSON_BIN}
        src/PoissonFENonlinearProblem.cpp
        src/main.cpp
)
# target_code_coverage(${POISSON_BIN})
target_sanitization(${POISSON_BIN})

target_include_directories(
    ${POISSON_BIN}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CONDA_INCLUDE_DIR}
        ${GODZILLA_INCLUDE_DIRS}
)

target_link_libraries(
    ${POISSON_BIN}
    PUBLIC
        ${GODZILLA_LIBRARIES}
)

# Tests

if (GODZILLA_BUILD_TESTS)
    add_subdirectory(test)
endif()
