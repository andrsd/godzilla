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


# libMesh

if(NOT METHOD)
    set(METHOD opt)
endif()

if(DEFINED ENV{LIBMESH_DIR})
    set(LIBMESH_DIR $ENV{LIBMESH_DIR})
else()
    set(LIBMESH_DIR ../moose)
endif()
set(LIBMESH_INCLUDE_DIR ${LIBMESH_DIR}/include)
find_library(LIBMESH_LIB mesh_${METHOD} ${LIBMESH_DIR}/lib)
find_library(TIMPI_LIB timpi_${METHOD} ${LIBMESH_DIR}/lib)


# MOOSE
if(DEFINED ENV{MOOSE_DIR})
    set(MOOSE_DIR $ENV{MOOSE_DIR})
else()
    set(MOOSE_DIR ../moose)
endif()
set(MOOSE_FRAMEWORK_DIR ${MOOSE_DIR}/framework)

set(MOOSE_INCLUDE_DIRS
    ${MOOSE_FRAMEWORK_DIR}/include
    # necessary MOOSE include evil
    ${MOOSE_FRAMEWORK_DIR}/include/actions
    ${MOOSE_FRAMEWORK_DIR}/include/base
    ${MOOSE_FRAMEWORK_DIR}/include/interfaces
    ${MOOSE_FRAMEWORK_DIR}/include/outputs
    ${MOOSE_FRAMEWORK_DIR}/include/parser
    ${MOOSE_FRAMEWORK_DIR}/include/reporters
    ${MOOSE_FRAMEWORK_DIR}/include/restart
    ${MOOSE_FRAMEWORK_DIR}/include/utils
    ${MOOSE_FRAMEWORK_DIR}/include/vectorpostprocessors

    ${MOOSE_FRAMEWORK_DIR}/contrib/boost/include
    ${MOOSE_FRAMEWORK_DIR}/contrib/json/include
    ${MOOSE_FRAMEWORK_DIR}/contrib/hit
)

find_library(MOOSE_LIB moose-${METHOD} ${MOOSE_FRAMEWORK_DIR})
find_library(MOOSE_HIT_LIB hit-${METHOD} ${MOOSE_FRAMEWORK_DIR}/contrib/hit)

# godzilla

set(GODZILLA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} SHARED
    src/base/CallStack.cpp
    src/base/GodzillaApp.cpp
    src/base/GPrintInterface.cpp
    src/executioners/GExecutioner.cpp
    src/input/GYMLFile.cpp
    src/grids/G1DLineMesh.cpp
    src/grids/G2DRectangleMesh.cpp
    src/grids/G2DStructuredGrid.cpp
    src/grids/GExodusIIMesh.cpp
    src/grids/GGrid.cpp
    src/grids/GUnstructuredMesh.cpp
    src/problems/GProblem.cpp
    src/problems/GPetscLinearProblem.cpp
    src/problems/GPetscNonlinearProblem.cpp
    src/problems/GPetscFENonlinearProblem.cpp
)
target_code_coverage(${PROJECT_NAME})
target_address_sanitization(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_MODE BATCH)

target_include_directories(${PROJECT_NAME} PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBMESH_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${MOOSE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GODZILLA_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PETSC_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBMESH_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${TIMPI_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MOOSE_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC ${MOOSE_HIT_LIB})


if (BUILD_EXAMPLES)
    add_subdirectory(examples)
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
