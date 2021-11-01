project(poisson)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    src/PoissonLinearProblem.cpp
    src/PoissonNonlinearProblem.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBMESH_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MOOSE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC godzilla)
