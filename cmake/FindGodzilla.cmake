# Find Godzilla
#
# Once done this will define
#  GODZILLA_FOUND - System has Godzilla
#  GODZILLA_INCLUDE_DIRS - The Godzilla include directories
#  GODZILLA_LIBRARIES - The Godzilla libraries

find_path(
    GODZILLA_INCLUDE
        Godzilla.h
    PATHS
        ${GODZILLA_INCLUDE_DIR}
)

find_library(
    GODZILLA_LIBRARY
        godzilla
    PATHS
        ${GODZILLA_BINARY_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Godzilla
    DEFAULT_MSG
    GODZILLA_LIBRARY
    GODZILLA_INCLUDE
)

mark_as_advanced(
    GODZILLA_INCLUDE
    GODZILLA_LIBRARY
)

set(GODZILLA_LIBRARIES
    ${GODZILLA_LIBRARY}
    ${PETSC_LIBRARY}
    muparser
    yaml-cpp
)
set(GODZILLA_INCLUDE_DIRS
    ${GODZILLA_INCLUDE}
    ${GODZILLA_CONTRIB_DIR}
    ${PETSC_INCLUDE_DIR}
    ${MUPARSER_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
)
