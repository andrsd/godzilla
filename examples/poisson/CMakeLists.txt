project(poisson)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    src/PoissonFENonlinearProblem.cpp
    src/PoissonLinearProblem.cpp
    src/PoissonNonlinearProblem.cpp
    src/X2Y2BC.cpp
    src/main.cpp
)
target_code_coverage(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_CONTRIB_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC godzilla)

add_test(${PROJECT_NAME}-linear ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/linear_poisson_2d.gyml)
add_test(${PROJECT_NAME}-nl     ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/nl_poisson_2d.gyml)
add_test(${PROJECT_NAME}-nl-fem ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/nl_poisson_2d_fe.gyml)

if(CODE_COVERAGE)
    set_tests_properties(${PROJECT_NAME}-linear PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}-linear.profraw)
    set_tests_properties(${PROJECT_NAME}-nl PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}-nl.profraw)
    set_tests_properties(${PROJECT_NAME}-nl-fem PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}-nl-fem.profraw)
endif()
