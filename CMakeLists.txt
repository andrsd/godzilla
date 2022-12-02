cmake_minimum_required(VERSION VERSION 3.16)

project(godzilla)

enable_language(C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitization.cmake)

option(GODZILLA_WITH_MPI "Build with MPI support" YES)
option(GODZILLA_WITH_PERF_LOG "Build with performance logging" NO)
option(GODZILLA_BUILD_EXAMPLES "Build examples" NO)
option(GODZILLA_BUILD_TESTS "Build tests" NO)

find_package(fmt REQUIRED)
find_package(yaml-cpp REQUIRED)
if (GODZILLA_WITH_MPI)
    find_package(MPI REQUIRED)
endif()

find_package(PETSc)
if (NOT ${PETSC_FOUND})
    message(STATUS "Try to set environment variable PETSC_DIR to point to your PETSc installation.")
endif()

if (GODZILLA_WITH_MPI)
    set(HDF5_PREFER_PARALLEL TRUE)
endif()
find_package(HDF5 COMPONENTS C)
if (NOT ${HDF5_FOUND})
    message(STATUS "HDF5 was not found.")
endif()

FetchContent_Declare(
    exodusIIcpp
    GIT_REPOSITORY https://github.com/andrsd/exodusIIcpp.git
    GIT_TAG        v1.2.0
)
mark_as_advanced(FORCE
    FETCHCONTENT_SOURCE_DIR_EXODUSIICPP
    FETCHCONTENT_UPDATES_DISCONNECTED_EXODUSIICPP
    EXODUSIICPP_BUILD_TESTS
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
mark_as_advanced(FORCE
    BUILD_GMOCK
    INSTALL_GTEST
    FETCHCONTENT_SOURCE_DIR_GOOGLETEST
    FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLETEST
)

FetchContent_MakeAvailable(exodusIIcpp googletest)
mark_as_advanced(FORCE
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_QUIET
    FETCHCONTENT_UPDATES_DISCONNECTED
)

# Godzilla

include(GNUInstallDirs)

add_subdirectory(contrib/muParser)
add_subdirectory(src)

# Install

configure_file(godzilla.pc.in godzilla.pc @ONLY)

install(
    FILES
        "${PROJECT_BINARY_DIR}/godzilla.pc"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Tests

if (GODZILLA_BUILD_TESTS)
    set(GODZILLA_UNIT_TESTS_ROOT ${CMAKE_SOURCE_DIR}/test CACHE PATH "" FORCE)
    mark_as_advanced(FORCE
        GODZILLA_UNIT_TESTS_ROOT
    )

    enable_testing()
    add_subdirectory(test)
endif()

# Examples

if (GODZILLA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Config file

configure_file(include/GodzillaConfig.h.in GodzillaConfig.h)

add_subdirectory(docs)
