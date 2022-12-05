include(CMakeFindDependencyMacro)

enable_language(C)
find_dependency(HDF5 REQUIRED)
find_dependency(fmt REQUIRED)
find_dependency(yaml-cpp REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/GodzillaTargets.cmake")
