# Find Judy
#
# Once done this will define
#  JUDY_FOUND - System has Judy
#  JUDY_INCLUDE_DIR - The Judy include directory
#  JUDY_LIBRARY - The Judy library

find_path(
    JUDY_INCLUDE_DIR
        Judy.h
    PATHS
        $ENV{JUDY_DIR}/include
)

find_library(
    JUDY_LIBRARY
        Judy
    PATHS
        $ENV{JUDY_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Judy
    DEFAULT_MSG
    JUDY_LIBRARY
    JUDY_INCLUDE_DIR
)
