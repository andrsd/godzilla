# Find PETSc
#
# Once done this will define
#  PETSC_FOUND - System has PETSc
#  PETSC_INCLUDE_DIR - The PETSc include directory
#  PETSC_LIBRARY - The PETSc library

find_path(
    PETSC_INCLUDE_DIR
        petsc.h
    PATHS
        $ENV{PETSC_DIR}/include
)

find_library(
    PETSC_LIBRARY
        petsc
    PATHS
        $ENV{PETSC_DIR}/$ENV{PETSC_ARCH}/lib
        $ENV{PETSC_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    PETSc
    DEFAULT_MSG
    PETSC_LIBRARY
    PETSC_INCLUDE_DIR
)
