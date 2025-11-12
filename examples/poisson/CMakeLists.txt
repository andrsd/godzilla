project(poisson)

add_executable(
    poisson
        src/PoissonApp.cpp
        src/PoissonEquation.cpp
        src/main.cpp
)

target_compile_options(
    poisson
    PRIVATE
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/include=include
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/src=godzilla/src
        -ffile-prefix-map=${PROJECT_SOURCE_DIR}=${PROJECT_NAME}
)

target_include_directories(
    poisson
    PUBLIC
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    poisson
    PRIVATE
        godzilla
)

if (GODZILLA_BUILD_TESTS)
    include(TesterExodusII)

    add_test_exodiff(
        NAME    poisson-mms-1d
        COMMAND poisson -d 1
        OUTPUT  ${PROJECT_BINARY_DIR}/mms-1d.exo
        GOLD    ${PROJECT_SOURCE_DIR}/gold/mms-1d.exo
        ARGS    -Floor 1e-16
    )

    add_test_exodiff(
        NAME    poisson-mms-2d
        COMMAND poisson -d 2
        OUTPUT  ${PROJECT_BINARY_DIR}/mms-2d.exo
        GOLD    ${PROJECT_SOURCE_DIR}/gold/mms-2d.exo
        ARGS    -Floor 1e-16
    )

    add_test_exodiff(
        NAME    poisson-mms-3d
        COMMAND poisson -d 3
        OUTPUT  ${PROJECT_BINARY_DIR}/mms-3d.exo
        GOLD    ${PROJECT_SOURCE_DIR}/gold/mms-3d.exo
        ARGS    -Floor 1e-16
    )
endif()
