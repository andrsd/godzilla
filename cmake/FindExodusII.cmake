# Find ExodusII
#
# Once done this will define
#  EXODUSII_FOUND - System has ExodusII
#  EXODUSII_INCLUDE_DIR - The ExodusII include directory
#  EXODUSII_LIBRARY - The ExodusII library

find_path(
    NETCDF_INCLUDE_DIR
        netcdf.h
    PATHS
        $ENV{NETCDF_DIR}/include
)

find_library(
    NETCDF_LIBRARY
        netcdf
    PATHS
        $ENV{NETCDF_DIR}/lib
)

# The HDF5 bit may be moved into its own FindHDF5.cmake
find_path(
    HDF5_INCLUDE_DIR
        hdf5.h
    PATHS
        $ENV{HDF5_DIR}/include
    PATH_SUFFIXES
        hdf5/openmpi
        hdf5/mpich
        hdf5/serial
)

find_library(
    HDF5_LIBRARY
        hdf5
    PATHS
        $ENV{HDF5_DIR}/lib
    PATH_SUFFIXES
        hdf5/openmpi
        hdf5/mpich
        hdf5/serial
)

find_path(
    EXODUSII_INCLUDE_DIR
        exodusII.h
    PATHS
        $ENV{EXODUSII_DIR}/include
)

find_library(
    EXODUSII_LIBRARY
        exoIIv2c
    PATHS
        $ENV{EXODUSII_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ExodusII
    DEFAULT_MSG
    EXODUSII_LIBRARY
    EXODUSII_INCLUDE_DIR
)
