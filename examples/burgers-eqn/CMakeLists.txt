project(burgers-eqn)

if (DEFINED ENV{CONDA_PREFIX})
    set(CMAKE_BUILD_RPATH $ENV{CONDA_PREFIX}/lib)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BURGERS_BIN ${PROJECT_NAME})

add_executable(
    ${BURGERS_BIN}
        src/BurgersEquation.cpp
        src/main.cpp
)
# target_code_coverage(${BURGERS_BIN})
target_sanitization(${BURGERS_BIN})

target_include_directories(
    ${BURGERS_BIN}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CONDA_INCLUDE_DIR}
        ${GODZILLA_INCLUDE_DIRS}
)

target_link_libraries(
    ${BURGERS_BIN}
    PUBLIC
        ${GODZILLA_LIBRARIES}
)

# Tests

if (GODZILLA_BUILD_TESTS)
    add_subdirectory(test)
endif()
