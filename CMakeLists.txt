cmake_minimum_required(VERSION VERSION 3.16)

project(godzilla VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FetchContent)
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
find_library(YAML_CPP_LIB yaml-cpp ${CONDA_LIB_DIR})

find_package(Doxygen)

# muparser

FetchContent_Declare(
    muparser
    GIT_REPOSITORY https://github.com/beltoforion/muparser.git
    # version 2.3.2
    GIT_TAG        207d5b77c05c9111ff51ab91082701221220c477
)
if(NOT muparser_POPULATED)
    FetchContent_Populate(muparser)
    set(ENABLE_SAMPLES OFF CACHE INTERNAL "Build samples")
    set(ENABLE_OPENMP OFF CACHE INTERNAL "Build with OpenMP")
    add_subdirectory(${muparser_SOURCE_DIR} ${muparser_BINARY_DIR})
    set(muparser_INCLUDE_DIR ${muparser_SOURCE_DIR}/include)
endif()

# godzilla

set(GODZILLA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(GODZILLA_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/contrib)

add_library(${PROJECT_NAME} SHARED
    # factory must be first
    src/Factory.cpp
    src/App.cpp
    src/BoundaryCondition.cpp
    src/CallStack.cpp
    src/CmdLineArgParser.cpp
    src/ConstantIC.cpp
    src/DirichletBC.cpp
    src/Executioner.cpp
    src/ExodusIIMesh.cpp
    src/FENonlinearProblem.cpp
    src/FEProblemInterface.cpp
    src/FunctionIC.cpp
    src/FunctionInterface.cpp
    src/Grid.cpp
    src/GYMLFile.cpp
    src/Init.cpp
    src/InitialCondition.cpp
    src/InputParameters.cpp
    src/LinearProblem.cpp
    src/LineMesh.cpp
    src/NonlinearProblem.cpp
    src/Object.cpp
    src/Output.cpp
    src/PrintInterface.cpp
    src/Problem.cpp
    src/RectangleMesh.cpp
    src/Steady.cpp
    src/StructuredGrid1D.cpp
    src/StructuredGrid2D.cpp
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
target_include_directories(${PROJECT_NAME} PRIVATE ${muparser_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${YAML_CPP_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PETSC_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC muparser)



if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Documentation

if(DOXYGEN_FOUND)
    if (NOT PROJECT_RST_DIR)
        set(PROJECT_RST_DIR ${PROJECT_BINARY_DIR}/rst)
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
            ${PROJECT_RST_DIR}/index.rst
            ${PROJECT_RST_DIR}/getting-started
            ${PROJECT_RST_DIR}/examples
            ${PROJECT_RST_DIR}/devel/index.rst
            ${PROJECT_RST_DIR}/conf.py
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_RST_DIR}/devel/index.rst
        COMMAND
            ${DOXYREST} -c doxyrest-config.lua
        DEPENDS
            ${PROJECT_BINARY_DIR}/xml/index.xml
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_RST_DIR}/getting-started
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/getting-started ${PROJECT_RST_DIR}/getting-started
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/getting-started
    )
    add_custom_command(
        OUTPUT
            ${PROJECT_RST_DIR}/examples
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/examples ${PROJECT_RST_DIR}/examples
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/examples
    )
    add_custom_command(
        OUTPUT
            ${PROJECT_RST_DIR}/index.rst
        COMMAND
            cmake -E copy ${PROJECT_SOURCE_DIR}/docs/index.rst ${PROJECT_RST_DIR}
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

# TESTING

if (BUILD_TESTS)
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

unset(PROJECT_RST_DIR CACHE)
