project(burgers-eqn)

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
)

if (GODZILLA_BUILD_TESTS)
    add_test(
        NAME ex-burgers-run
        COMMAND ${PROJECT_NAME}
    )
    add_test(
        NAME ex-burgers-diff
        COMMAND
            exodiff -Floor 1e-16
                ${PROJECT_BINARY_DIR}/burgers.exo
                ${PROJECT_SOURCE_DIR}/gold/burgers.exo
    )
    set_tests_properties(
        ex-burgers-diff
        PROPERTIES
            DEPENDS ex-burgers-run
    )
endif()
