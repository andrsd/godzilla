project(heat-eqn)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Godzilla)

add_executable(${PROJECT_NAME}
    src/ConvectiveHeatFluxConstBC.cpp
    src/HeatEquationProblem.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CONDA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GODZILLA_LIBRARIES})

add_test(${PROJECT_NAME}-2d ${PROJECT_NAME} -i ${PROJECT_SOURCE_DIR}/gyml/2d.gyml)
