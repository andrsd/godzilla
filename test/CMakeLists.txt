project(godzilla-test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    src/base/GodzillaAppTest.cpp
    src/grids/G1DLineMeshTest.cpp
    src/main.cpp
)
target_code_coverage(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBMESH_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MOOSE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/test/include)


target_link_libraries(${PROJECT_NAME} PUBLIC
    godzilla
    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_BOTH_LIBRARIES}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
