project(heat-eqn)

macro(add_binny name)
    add_executable(${PROJECT_NAME}-${name} ${ARGN})
    target_include_directories(${PROJECT_NAME}-${name}
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
            ${CMAKE_BINARY_DIR}
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/contrib
    )
    target_link_libraries(${PROJECT_NAME}-${name} PRIVATE godzilla)
endmacro()

# MMS 1D

add_binny(mms-1d
    src/HeatEquationProblem.cpp
    src/mms-1d.cpp
)

# 2D

add_binny(2d
    src/HeatEquationProblem.cpp
    src/ConvectiveHeatFluxBC.cpp
    src/2d.cpp
)

# 2D-explicit

add_binny(2d-explicit
    src/HeatEquationExplicit.cpp
    src/2d-explicit.cpp
)

# TESTS

if (GODZILLA_BUILD_TESTS)
    include(TesterExodusII)

    add_test_exodiff(
        NAME    ${PROJECT_NAME}-mms-1d
        COMMAND ${PROJECT_NAME}-mms-1d
        OUTPUT  ${PROJECT_BINARY_DIR}/mms-1d.exo
        GOLD    ${PROJECT_SOURCE_DIR}/gold/mms-1d.exo
        ARGS    -Floor 1e-12
    )

    add_test_exodiff(
        NAME    ${PROJECT_NAME}-2d
        COMMAND ${PROJECT_NAME}-2d
        OUTPUT  ${PROJECT_BINARY_DIR}/2d.exo
        GOLD    ${PROJECT_SOURCE_DIR}/gold/2d.exo
        ARGS    -Floor 1e-12
    )

    add_test_exodiff(
        NAME    ${PROJECT_NAME}-2d-explicit
        COMMAND ${PROJECT_NAME}-2d-explicit
        OUTPUT  ${PROJECT_BINARY_DIR}/2d-explicit.exo
        GOLD    ${PROJECT_SOURCE_DIR}/gold/2d-explicit.exo
        ARGS    -Floor 1e-12
    )
endif()
