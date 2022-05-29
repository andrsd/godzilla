project(heat-eqn)

if (DEFINED ENV{CONDA_PREFIX})
    set(CMAKE_BUILD_RPATH $ENV{CONDA_PREFIX}/lib)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    ${PROJECT_NAME}
        src/ConvectiveHeatFluxBC.cpp
        src/HeatEquationProblem.cpp
        src/HeatEquationExplicit.cpp
        src/main.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CONDA_INCLUDE_DIR}
        ${GODZILLA_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        ${GODZILLA_LIBRARIES}
)

# Tests

if (GODZILLA_BUILD_TESTS)
    add_subdirectory(test)
endif()
