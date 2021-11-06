cmake_minimum_required(VERSION 3.16)

project(godzilla VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/AddressSanitization.cmake)
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
    src/base/App.cpp
    src/base/CallStack.cpp
    src/base/Factory.cpp
    src/base/GPrintInterface.cpp
    src/base/Init.cpp
    src/base/Object.cpp
    src/executioners/GExecutioner.cpp
    src/input/GYMLFile.cpp
    src/input/CmdLineArgParser.cpp
    src/grids/G1DLineMesh.cpp
    src/grids/G2DRectangleMesh.cpp
    src/grids/G2DStructuredGrid.cpp
    src/grids/GExodusIIMesh.cpp
    src/grids/GGrid.cpp
    src/grids/GUnstructuredMesh.cpp
    src/outputs/GOutput.cpp
    src/outputs/GVTKOutput.cpp
    src/problems/GProblem.cpp
    src/problems/GPetscLinearProblem.cpp
    src/problems/GPetscNonlinearProblem.cpp
    src/problems/GPetscFENonlinearProblem.cpp
    src/utils/InputParameters.cpp
    src/utils/Utils.cpp
)
target_code_coverage(${PROJECT_NAME})
target_address_sanitization(${PROJECT_NAME})

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
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/rst/index.rst
        COMMAND
            ${DOXYREST} -c doxyrest-config.lua
        DEPENDS
            ${PROJECT_BINARY_DIR}/xml/index.xml
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

enable_testing()
if (BUILD_TESTS)
    find_package(GTest REQUIRED)
    find_package(GMock REQUIRED)
    add_subdirectory(test)
    set(UNIT_TESTS_ROOT ${CMAKE_SOURCE_DIR}/test)
endif()

configure_file(include/base/GodzillaConfig.h.in GodzillaConfig.h)
