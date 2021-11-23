cmake_minimum_required(VERSION VERSION 3.16)

project(godzilla VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitization.cmake)

option(GODZILLA_BUILD_EXAMPLES "Build examples" NO)
option(GODZILLA_BUILD_TESTS "Build tests" NO)

# PETSc

if(DEFINED ENV{PETSC_DIR})
    set(PETSC_DIR $ENV{PETSC_DIR})
    message(STATUS "PETSC_DIR: " ${PETSC_DIR})
else()
    message(FATAL_ERROR "Environment variable PETSC_DIR not set.  Set the variable in your environment to point to your PETSc installation.")
endif()
set(PETSC_INCLUDE_DIR ${PETSC_DIR}/include)
if(DEFINED ENV{PETSC_ARCH})
    find_library(PETSC_LIB petsc ${PETSC_DIR}/$ENV{PETSC_ARCH}/lib)
else()
    find_library(PETSC_LIB petsc ${PETSC_DIR}/lib)
endif()

# muParser
FetchContent_Declare(
    muparser
    GIT_REPOSITORY https://github.com/beltoforion/muparser.git
    GIT_TAG        11410c968cb5ecf20f9a898b83af91984e8307d4
)
set(ENABLE_SAMPLES OFF CACHE BOOL "" FORCE)
set(ENABLE_OPENMP OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# yamp-cpp
FetchContent_Declare(
    yaml_cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        yaml-cpp-0.7.0
)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

# Google test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# external deps

FetchContent_MakeAvailable(muparser yaml_cpp googletest)

set(MUPARSER_INCLUDE_DIR ${muparser_SOURCE_DIR}/include)
set(YAML_CPP_INCLUDE_DIR ${yaml_cpp_SOURCE_DIR}/include)

# godzilla

set(GODZILLA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(GODZILLA_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/contrib)

add_library(${PROJECT_NAME} SHARED
    # factory must be first
    src/Factory.cpp
    src/App.cpp
    src/BoundaryCondition.cpp
    src/BoxMesh.cpp
    src/CallStack.cpp
    src/CmdLineArgParser.cpp
    src/ConstantIC.cpp
    src/DirichletBC.cpp
    src/ExodusIIMesh.cpp
    src/FENonlinearProblem.cpp
    src/FEProblemInterface.cpp
    src/Function.cpp
    src/FunctionEvaluator.cpp
    src/FunctionIC.cpp
    src/FunctionInterface.cpp
    src/Grid.cpp
    src/GYMLFile.cpp
    src/HDF5Output.cpp
    src/ImplicitFENonlinearProblem.cpp
    src/Init.cpp
    src/InitialCondition.cpp
    src/InputParameters.cpp
    src/LinearInterpolation.cpp
    src/LinearProblem.cpp
    src/LineMesh.cpp
    src/NonlinearProblem.cpp
    src/Object.cpp
    src/Output.cpp
    src/PrintInterface.cpp
    src/Problem.cpp
    src/RectangleMesh.cpp
    src/StructuredGrid1D.cpp
    src/StructuredGrid2D.cpp
    src/TransientInterface.cpp
    src/UnstructuredMesh.cpp
    src/Utils.cpp
    src/VTKOutput.cpp
)
target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH)

target_include_directories(${PROJECT_NAME} PRIVATE ${CONDA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${HDF5_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_CONTRIB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MUPARSER_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${YAML_CPP_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC muparser)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PETSC_LIB})

# Tests

if (GODZILLA_BUILD_TESTS)
    set(GODZILLA_UNIT_TESTS_ROOT ${CMAKE_SOURCE_DIR}/test CACHE PATH "" FORCE)

    enable_testing()
    add_subdirectory(test)
endif()

# Examples

if (GODZILLA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Config file

configure_file(include/GodzillaConfig.h.in GodzillaConfig.h)

# Documentation

find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    if (NOT GODZILLA_RST_DIR)
        set(GODZILLA_RST_DIR ${PROJECT_BINARY_DIR}/rst)
    endif()
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
            ${GODZILLA_RST_DIR}/index.rst
            ${GODZILLA_RST_DIR}/getting-started
            ${GODZILLA_RST_DIR}/examples
            ${GODZILLA_RST_DIR}/devel/index.rst
            ${GODZILLA_RST_DIR}/conf.py
    )

    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/devel/index.rst
        COMMAND
            ${DOXYREST} -c doxyrest-config.lua
        DEPENDS
            ${PROJECT_BINARY_DIR}/xml/index.xml
    )

    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/getting-started
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/getting-started ${GODZILLA_RST_DIR}/getting-started
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/getting-started
    )
    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/examples
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/examples ${GODZILLA_RST_DIR}/examples
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/examples
    )
    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/index.rst
        COMMAND
            cmake -E copy ${PROJECT_SOURCE_DIR}/docs/index.rst ${GODZILLA_RST_DIR}
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/index.rst
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

unset(GODZILLA_RST_DIR CACHE)
