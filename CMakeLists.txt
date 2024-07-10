cmake_minimum_required(VERSION 3.16)

project(godzilla
    VERSION 0.7
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitization.cmake)

set(BuildValues "Release;Debug;RelWithDebInfo;MinSizeRel")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BuildValues})

option(GODZILLA_WITH_MPI "Build with MPI support" YES)
option(GODZILLA_WITH_PERF_LOG "Build with performance logging" NO)
option(GODZILLA_BUILD_EXAMPLES "Build examples" NO)
option(GODZILLA_BUILD_TESTS "Build tests" NO)

find_package(fmt 8.0 REQUIRED)
find_package(yaml-cpp 0.8 REQUIRED)
if (GODZILLA_WITH_MPI)
    find_package(MPI REQUIRED)
endif()
find_package(PETSc 3.20 REQUIRED)
if (NOT ${PETSC_FOUND})
    message(STATUS "Try to set environment variable PETSC_DIR to point to your PETSc installation.")
endif()
find_package(exodusIIcpp 2.0 REQUIRED)

if (GODZILLA_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.12.0
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    mark_as_advanced(FORCE
        BUILD_GMOCK
        INSTALL_GTEST
        FETCHCONTENT_SOURCE_DIR_GOOGLETEST
        FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLETEST
    )

    FetchContent_MakeAvailable(googletest)
    mark_as_advanced(FORCE
        FETCHCONTENT_BASE_DIR
        FETCHCONTENT_FULLY_DISCONNECTED
        FETCHCONTENT_QUIET
        FETCHCONTENT_UPDATES_DISCONNECTED
    )
endif ()

# godzilla

add_subdirectory(contrib/muParser)
add_subdirectory(src)
add_subdirectory(tools)

# Install

configure_file(godzilla.pc.in godzilla.pc @ONLY)

install(
    FILES "${PROJECT_BINARY_DIR}/godzilla.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
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

configure_file(include/Config.h.in godzilla/Config.h)

add_subdirectory(docs)
