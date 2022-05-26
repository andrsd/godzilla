include(CMakeFindDependencyMacro)

find_dependency(HDF5 REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/GodzillaTargets.cmake")
