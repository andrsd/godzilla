cmake_minimum_required(VERSION VERSION 3.16)

project(godzilla)

enable_language(C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitization.cmake)

option(GODZILLA_BUILD_EXAMPLES "Build examples" NO)
option(GODZILLA_BUILD_TESTS "Build tests" NO)

find_package(fmt REQUIRED)
find_package(yaml-cpp REQUIRED)

find_package(PETSc)
if (NOT ${PETSC_FOUND})
    message(STATUS "Try to set environemnt variable PETSC_DIR to point to your PETSc installation.")
endif()

find_package(HDF5 COMPONENTS C)
if (NOT ${HDF5_FOUND})
    message(STATUS "HDF5 was not found.")
endif()

find_package(NetCDF)
if (NOT ${NETCDF_FOUND})
    message(STATUS "Try to set environemnt variable NETCDF_DIR to point to your NetCDF installation.")
endif()

find_package(ExodusII)
if (NOT ${EXODUSII_FOUND})
    message(STATUS "Try to set environemnt variable EXODUSII_DIR to point to your ExodusII installation.")
endif()


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

FetchContent_MakeAvailable(googletest)
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
