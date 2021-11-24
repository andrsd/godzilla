project(poisson-test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    ../src/PoissonFENonlinearProblem.cpp
    src/PoissonFE_test.cpp
    src/main.cpp
)
target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_CONTRIB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CONDA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MUPARSER_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${YAML_CPP_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../include)

target_link_libraries(${PROJECT_NAME} PUBLIC godzilla)
target_link_libraries(${PROJECT_NAME} PUBLIC muparser)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC gmock_main)

add_test(NAME poisson-test COMMAND ${PROJECT_NAME})
if(GODZILLA_CODE_COVERAGE)
    set_tests_properties(poisson-test PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}.profraw)
endif()
