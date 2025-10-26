project(advect-eqn)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_sanitization(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        godzilla
        PETSc::petsc
)

if (GODZILLA_BUILD_TESTS)
    include(TesterExodusII)

    if(${PETSC_VERSION} VERSION_LESS "3.21.0")
        add_test_exodiff(
            NAME    advect
            COMMAND ${PROJECT_NAME}
            OUTPUT  ${PROJECT_BINARY_DIR}/1d.exo
            GOLD    ${PROJECT_SOURCE_DIR}/gold/1d.exo
            ARGS    -Floor 1e-16
        )
    endif()
endif()
