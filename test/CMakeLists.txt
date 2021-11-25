project(godzilla-test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Godzilla)

add_executable(${PROJECT_NAME}
    src/BoundaryCondition_test.cpp
    src/BoxMesh_test.cpp
    src/CallStack_test.cpp
    src/ConstantIC_test.cpp
    src/DirichletBC_test.cpp
    src/ExodusIIMesh_test.cpp
    src/Factory_test.cpp
    src/FENonlinearProblem_test.cpp
    src/FunctionIC_test.cpp
    src/GodzillaApp_test.cpp
    src/GYMLFile_test.cpp
    src/HDF5Output_test.cpp
    src/ImplicitFENonlinearProblem_test.cpp
    src/InputParameters_test.cpp
    src/LinearInterpolation_test.cpp
    src/LinearProblem_test.cpp
    src/LineMesh_test.cpp
    src/NonlinearProblem_test.cpp
    src/Object_test.cpp
    src/PiecewiseLinear_test.cpp
    src/PrintInterface_test.cpp
    src/RectangleMesh_test.cpp
    src/StructuredGrid1D_test.cpp
    src/StructuredGrid2D_test.cpp
    src/Utils_test.cpp
    src/VTKOutput_test.cpp
    src/main.cpp
)
target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CONDA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/test/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GODZILLA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC gmock_main)

add_test(NAME godzilla-test COMMAND ${PROJECT_NAME})
if(GODZILLA_CODE_COVERAGE)
    set_tests_properties(godzilla-test PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}.profraw)
endif()
