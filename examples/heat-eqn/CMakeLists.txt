project(heat-eqn)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    src/HeatEquationProblem.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CONDA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_CONTRIB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${muparser_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC godzilla)
target_link_libraries(${PROJECT_NAME} PUBLIC muparser)

add_test(${PROJECT_NAME}-2d ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/2d.gyml)
