cmake_minimum_required(VERSION VERSION 3.16)

project(godzilla VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitization.cmake)
option(BUILD_EXAMPLES "Build examples" NO)
option(BUILD_TESTS "Build tests" NO)

# PETSc

if(DEFINED ENV{PETSC_DIR})
    set(PETSC_DIR $ENV{PETSC_DIR})
    message(STATUS "PETSC_DIR: " ${PETSC_DIR})
else()
    message(FATAL_ERROR "Environment variable PETSC_DIR not set.  Set the variable in your environment to point to your PETSc installation.")
endif()
set(PETSC_INCLUDE_DIR ${PETSC_DIR}/include)
find_library(PETSC_LIB petsc ${PETSC_DIR}/lib)
find_package(Doxygen)

# godzilla

set(GODZILLA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(GODZILLA_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/contrib)

add_library(${PROJECT_NAME} SHARED
    src/App.cpp
    src/CallStack.cpp
    src/Factory.cpp
    src/FEProblemInterface.cpp
    src/FunctionInterface.cpp
    src/PrintInterface.cpp
    src/Init.cpp
    src/InitialCondition.cpp
    src/Object.cpp
    src/Executioner.cpp
    src/GYMLFile.cpp
    src/CmdLineArgParser.cpp
    src/G1DLineMesh.cpp
    src/StructuredGrid1D.cpp
    src/G2DRectangleMesh.cpp
    src/StructuredGrid2D.cpp
    src/ExodusIIMesh.cpp
    src/Grid.cpp
    src/UnstructuredMesh.cpp
    src/Output.cpp
    src/VTKOutput.cpp
    src/FENonlinearProblem.cpp
    src/LinearProblem.cpp
    src/NonlinearProblem.cpp
    src/Problem.cpp
    src/ConstantIC.cpp
    src/InputParameters.cpp
    src/BoundaryCondition.cpp
    src/Utils.cpp
)
target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH)

target_include_directories(${PROJECT_NAME} PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_CONTRIB_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PETSC_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${TIMPI_LIB})


if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Documentation

if(DOXYGEN_FOUND)
    set(DOXYREST_PATH $ENV{DOXYREST_PATH})
    set(DOXYGEN_GENERATE_HTML NO)
    set(DOXYGEN_GENERATE_XML YES)
    set(DOXYGEN_STRIP_FROM_INC_PATH
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
    )
    set(DOXYGEN_STRIP_FROM_PATH
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
    )

    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        STRIP_FROM_PATH
        COMMENT "Generate doxygen XML"
    )

    find_program(DOXYREST doxyrest)
    find_program(SPHINX_BUILD sphinx-build)

    configure_file(docs/doxyrest-config.lua.in doxyrest-config.lua)
    configure_file(docs/conf.py rst/conf.py)

    add_custom_target(doc DEPENDS ${PROJECT_BINARY_DIR}/html/index.html)

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/html/index.html
        COMMAND
            ${SPHINX_BUILD} -b html rst html
        DEPENDS
            ${PROJECT_BINARY_DIR}/rst/index.rst
            ${PROJECT_BINARY_DIR}/rst/getting-started
            ${PROJECT_BINARY_DIR}/rst/examples
            ${PROJECT_BINARY_DIR}/rst/devel/index.rst
            ${PROJECT_BINARY_DIR}/rst/conf.py
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/rst/devel/index.rst
        COMMAND
            ${DOXYREST} -c doxyrest-config.lua
        DEPENDS
            ${PROJECT_BINARY_DIR}/xml/index.xml
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/rst/getting-started
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/getting-started ${PROJECT_BINARY_DIR}/rst/getting-started
    )
    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/rst/examples
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/examples ${PROJECT_BINARY_DIR}/rst/examples
    )
    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/rst/index.rst
        COMMAND
            cmake -E copy ${PROJECT_SOURCE_DIR}/docs/index.rst ${PROJECT_BINARY_DIR}/rst
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/xml/index.xml
        COMMAND
            doxygen Doxyfile.doxygen
        DEPENDS
            ${PROJECT_BINARY_DIR}/Doxyfile.doxygen
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/src
    )

endif()

# TESTING

if (BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      # version 1.10
      GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(test)
    set(UNIT_TESTS_ROOT ${CMAKE_SOURCE_DIR}/test)
endif()

configure_file(include/GodzillaConfig.h.in GodzillaConfig.h)
