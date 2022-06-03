include(CMakeFindDependencyMacro)

find_dependency(HDF5 REQUIRED)
find_package(Boost
    REQUIRED
    COMPONENTS
        filesystem
)

include("${CMAKE_CURRENT_LIST_DIR}/GodzillaTargets.cmake")
