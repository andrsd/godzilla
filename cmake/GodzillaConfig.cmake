include(CMakeFindDependencyMacro)

find_dependency(HDF5 REQUIRED)
find_package(Boost
    REQUIRED
    COMPONENTS
        filesystem
)
find_dependency(yaml-cpp REQUIRED)
find_dependency(muparser REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/GodzillaTargets.cmake")
