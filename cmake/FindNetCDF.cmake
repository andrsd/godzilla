# Find NetCDF
#
# Once done this will define
#  NETCDF_FOUND - System has ExodusII
#  NETCDF_INCLUDE_DIR - The ExodusII include directory
#  NETCDF_LIBRARY - The ExodusII library

find_path(
    NETCDF_INCLUDE_DIR
        netcdf.h
    PATHS
        $ENV{NETCDF_DIR}/include
        $ENV{PETSC_DIR}/include
)

find_library(
    NETCDF_LIBRARY
        netcdf
    PATHS
        $ENV{NETCDF_DIR}/lib
        $ENV{PETSC_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    NetCDF
    DEFAULT_MSG
    NETCDF_LIBRARY
    NETCDF_INCLUDE_DIR
)
