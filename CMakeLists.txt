cmake_minimum_required(VERSION VERSION 3.16)

project(
    godzilla
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitization.cmake)

option(GODZILLA_BUILD_EXAMPLES "Build examples" NO)
option(GODZILLA_BUILD_TESTS "Build tests" NO)

# ExodusII
find_package(ExodusII)
if (NOT ${EXODUSII_FOUND})
    message(STATUS "Try to set environemnt variable EXODUSII_DIR to point to your ExodusII installation.")
endif()

# PETSc
find_package(PETSc)
if (NOT ${PETSC_FOUND})
    message(STATUS "Try to set environemnt variable PETSC_DIR to point to your PETSc installation.")
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
  GIT_TAG        release-1.11.0
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# external deps

FetchContent_MakeAvailable(muparser yaml_cpp googletest)

set(MUPARSER_INCLUDE_DIR ${muparser_SOURCE_DIR}/include)
set(YAML_CPP_INCLUDE_DIR ${yaml_cpp_SOURCE_DIR}/include)

# Godzilla

set(GODZILLA_LIBRARIES
    ${NETCDF_LIBRARY}
    ${HDF5_LIBRARY}
    ${EXODUSII_LIBRARY}
    ${PETSC_LIBRARY}
    godzilla
    muparser
    yaml-cpp
)
set(GODZILLA_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/contrib
    ${EXODUSII_INCLUDE_DIR}
    ${PETSC_INCLUDE_DIR}
    ${MUPARSER_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
)

add_subdirectory(src)

# Install

include(GNUInstallDirs)
install(
    TARGETS
        godzilla
        yaml-cpp
        muparser
    EXPORT
        GodzillaTargets
    LIBRARY DESTINATION
        lib
    ARCHIVE DESTINATION
        lib
    RUNTIME DESTINATION
        bin
    INCLUDES DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    PUBLIC_HEADER DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
)

install(
    DIRECTORY
        include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING
        PATTERN "*.h"
)

install(
    DIRECTORY
        contrib/tclap
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING
        PATTERN "*.h"
)


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    GodzillaConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)

install(
    EXPORT
        GodzillaTargets
    FILE
        GodzillaTargets.cmake
    NAMESPACE
        Godzilla::
    DESTINATION
        lib/cmake/godzilla
)

install(
    FILES
        "${PROJECT_SOURCE_DIR}/cmake/GodzillaConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/GodzillaConfigVersion.cmake"
    DESTINATION
        lib/cmake/godzilla
)

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

add_subdirectory(docs)
