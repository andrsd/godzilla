project(godzilla-test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    src/base/GodzillaTestApp.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBMESH_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MOOSE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/test/include)

target_link_libraries(${PROJECT_NAME} PUBLIC godzilla)

# Tests

set(TESTS_DIR ${CMAKE_SOURCE_DIR}/test/tests)

add_test(cmd-line-version ${PROJECT_NAME} --version)
add_test(cmd-line-help ${PROJECT_NAME} --help)

add_test(input ${PROJECT_NAME} -i ${TESTS_DIR}/base/gyml/test.gyml)
