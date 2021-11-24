project(poisson)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    src/PoissonFENonlinearProblem.cpp
    src/PoissonLinearProblem.cpp
    src/PoissonNonlinearProblem.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CONDA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_CONTRIB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MUPARSER_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${YAML_CPP_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC godzilla)
target_link_libraries(${PROJECT_NAME} PUBLIC muparser)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)

add_test(${PROJECT_NAME}-lin ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/lin_2d.gyml)
add_test(${PROJECT_NAME}-nl  ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/nl_2d.gyml)
add_test(${PROJECT_NAME}-fem-1d ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/fe_1d.gyml)
add_test(${PROJECT_NAME}-fem-2d ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/fe_2d.gyml)
add_test(${PROJECT_NAME}-fem-3d ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/fe_3d.gyml)
