cmake_minimum_required(VERSION 3.16)

project(godzilla VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


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
configure_file(include/base/GodzillaRevision.h.in GodzillaRevision.h)

set(GODZILLA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_library(godzilla SHARED
    src/base/CallStack.cpp
    src/base/GodzillaApp.cpp
    src/base/GPrintInterface.cpp
    src/executioners/GExecutioner.cpp
    src/input/GYMLFile.cpp
    src/mesh/G1DLineMesh.cpp
    src/mesh/G2DRectangleMesh.cpp
    src/mesh/GMesh.cpp
    src/mesh/GExodusIIMesh.cpp
    src/problems/GProblem.cpp
)

set_target_properties(godzilla PROPERTIES UNITY_BUILD_MODE BATCH)

target_include_directories(godzilla PRIVATE ${PETSC_INCLUDE_DIR})
target_include_directories(godzilla PRIVATE ${LIBMESH_INCLUDE_DIR})
target_include_directories(godzilla PRIVATE ${MOOSE_INCLUDE_DIRS})
target_include_directories(godzilla PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(godzilla PRIVATE ${GODZILLA_INCLUDE_DIR})

target_link_libraries(godzilla PUBLIC yaml-cpp)
target_link_libraries(godzilla PUBLIC ${PETSC_LIB})
target_link_libraries(godzilla PUBLIC ${LIBMESH_LIB})
target_link_libraries(godzilla PUBLIC ${TIMPI_LIB})
target_link_libraries(godzilla PUBLIC ${MOOSE_LIB})
target_link_libraries(godzilla PUBLIC ${MOOSE_HIT_LIB})


# TESTING

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(test)
endif()
