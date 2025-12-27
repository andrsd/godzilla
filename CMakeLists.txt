cmake_minimum_required(VERSION 3.16)

project(godzilla
    VERSION 0.11.0
    LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(CheckIncludeFileCXX)
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
option(GODZILLA_WITH_TECIOCPP "Build with teciocpp support" NO)
option(GODZILLA_BUILD_EXAMPLES "Build examples" NO)
option(GODZILLA_BUILD_TESTS "Build tests" NO)

find_package(fmt 11 REQUIRED)
find_package(yaml-cpp 0.8 REQUIRED)
if (GODZILLA_WITH_MPI)
    find_package(MPI REQUIRED)
endif()
find_package(PETSc 3.23 REQUIRED)
find_package(exodusIIcpp 3 REQUIRED)
find_package(mpicpp-lite 2.2 REQUIRED)
find_package(HDF5 1.12 REQUIRED COMPONENTS C)
if (GODZILLA_WITH_TECIOCPP)
    find_package(teciocpp REQUIRED)
endif()

check_include_file_cxx(cxxabi.h HAVE_CXXABI_H)

# godzilla

add_subdirectory(src)
add_subdirectory(tools)

# Install

configure_file(godzilla.pc.in godzilla.pc @ONLY)

install(
    FILES "${PROJECT_BINARY_DIR}/godzilla.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
    FILES
        "${CMAKE_SOURCE_DIR}/cmake/FindPETSc.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
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


add_subdirectory(docs)
