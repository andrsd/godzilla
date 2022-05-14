include(CMakeFindDependencyMacro)

find_dependency(PETSc REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/GodzillaTargets.cmake")
