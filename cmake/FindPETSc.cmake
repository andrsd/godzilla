# Find PETSc
#
# Once done this will define
#  PETSC_FOUND - System has PETSc
#  PETSC_INCLUDE_DIR - The PETSc include directory
#  PETSC_LIBRARY - The PETSc library
#  PETSC_VERSION - The PETSc version

include(CheckSymbolExists)

find_package(PkgConfig)
pkg_check_modules(PC_PETSC QUIET PETSc)

find_path(PETSC_INCLUDE_DIR
    NAMES petsc.h
    PATHS
        ${PC_PETSC_INCLUDE_DIRS}
)

find_library(PETSC_LIBRARY
    NAMES petsc
    PATHS
        ${PC_PETSC_LIBRARY_DIRS}
)

set(PETSC_VERSION "unknown")
find_file(PETSCVERSION_H petscversion.h
    PATHS
        ${PC_PETSC_INCLUDE_DIRS}
)
if (PETSCVERSION_H)
    file(READ ${PETSCVERSION_H} PETSC_VERSION_FILE)
    string(REGEX MATCH "define[ ]+PETSC_VERSION_MAJOR[ ]+([0-9]+)" TMP "${PETSC_VERSION_FILE}")
    set(PETSC_VERSION_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "define[ ]+PETSC_VERSION_MINOR[ ]+([0-9]+)" TMP "${PETSC_VERSION_FILE}")
    set(PETSC_VERSION_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "define[ ]+PETSC_VERSION_SUBMINOR[ ]+([0-9]+)" TMP "${PETSC_VERSION_FILE}")
    set(PETSC_VERSION_PATCH ${CMAKE_MATCH_1})
    set(PETSC_VERSION "${PETSC_VERSION_MAJOR}.${PETSC_VERSION_MINOR}.${PETSC_VERSION_PATCH}")
endif()

set(PETSCCONF_H "${PETSC_INCLUDE_DIR}/petscconf.h")
check_symbol_exists(PETSC_HAVE_OPENCL ${PETSCCONF_H} PETSC_HAVE_OPENCL)
if (PETSC_HAVE_OPENCL)
    find_package(OpenCL REQUIRED)
endif()
check_symbol_exists(PETSC_HAVE_HYPRE ${PETSCCONF_H} PETSC_HAVE_HYPRE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    PETSc
    FOUND_VAR PETSC_FOUND
    REQUIRED_VARS PETSC_LIBRARY PETSC_INCLUDE_DIR
    VERSION_VAR PETSC_VERSION
)

if (PETSC_FOUND AND NOT TARGET PETSc::petsc)
    add_library(PETSc::petsc UNKNOWN IMPORTED)
    set_target_properties(PETSc::petsc
        PROPERTIES
            IMPORTED_LOCATION "${PETSC_LIBRARY}"
            INTERFACE_COMPILE_OPTIONS "${PC_PETSC_CFLAGS_OTHER}"
            INTERFACE_INCLUDE_DIRECTORIES "${PETSC_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
    PETSC_INCLUDE_DIR
    PETSC_LIBRARY
    PETSCVERSION_H
)
