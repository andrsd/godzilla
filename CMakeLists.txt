cmake_minimum_required(VERSION 3.16)
include(ProcessorCount)
ProcessorCount(PROCS)

project(godzilla VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT METHOD)
    set(METHOD opt)
endif()

# libMesh
if(DEFINED ENV{LIBMESH_DIR})
    set(LIBMESH_DIR $ENV{LIBMESH_DIR})
else()
    set(LIBMESH_DIR ../moose)
endif()
set(LIBMESH_INCLUDE_DIR ${LIBMESH_DIR}/include)
find_library(LIBMESH_LIB mesh_${METHOD} ${LIBMESH_DIR}/lib)
find_library(TIMPI_LIB timpi_${METHOD} ${LIBMESH_DIR}/lib)

# MOOSE
if(DEFINED ENV{MOOSE_DIR})
    set(MOOSE_DIR $ENV{MOOSE_DIR})
else()
    set(MOOSE_DIR ../moose)
endif()
set(MOOSE_FRAMEWORK_DIR ${MOOSE_DIR}/framework)

set(MOOSE_INCLUDE_DIRS
    ${MOOSE_FRAMEWORK_DIR}/include/actions
    ${MOOSE_FRAMEWORK_DIR}/include/auxkernels
    ${MOOSE_FRAMEWORK_DIR}/include/base
    ${MOOSE_FRAMEWORK_DIR}/include/bcs
    ${MOOSE_FRAMEWORK_DIR}/include/constraints
    ${MOOSE_FRAMEWORK_DIR}/include/controls
    ${MOOSE_FRAMEWORK_DIR}/include/dampers
    ${MOOSE_FRAMEWORK_DIR}/include/dgkernels
    ${MOOSE_FRAMEWORK_DIR}/include/dirackernels
    ${MOOSE_FRAMEWORK_DIR}/include/distributions
    ${MOOSE_FRAMEWORK_DIR}/include/executioners
    ${MOOSE_FRAMEWORK_DIR}/include/functions
    ${MOOSE_FRAMEWORK_DIR}/include/fvbcs
    ${MOOSE_FRAMEWORK_DIR}/include/fviks
    ${MOOSE_FRAMEWORK_DIR}/include/fvkernels
    ${MOOSE_FRAMEWORK_DIR}/include/geomsearch
    ${MOOSE_FRAMEWORK_DIR}/include/ics
    ${MOOSE_FRAMEWORK_DIR}/include/indicators
    ${MOOSE_FRAMEWORK_DIR}/include/interfacekernels
    ${MOOSE_FRAMEWORK_DIR}/include/interfaces
    ${MOOSE_FRAMEWORK_DIR}/include/kernels
    ${MOOSE_FRAMEWORK_DIR}/include/limiters
    ${MOOSE_FRAMEWORK_DIR}/include/linesearch
    ${MOOSE_FRAMEWORK_DIR}/include/loops
    ${MOOSE_FRAMEWORK_DIR}/include/markers
    ${MOOSE_FRAMEWORK_DIR}/include/materials
    ${MOOSE_FRAMEWORK_DIR}/include/mesh
    ${MOOSE_FRAMEWORK_DIR}/include/meshgenerators
    ${MOOSE_FRAMEWORK_DIR}/include/multiapps
    ${MOOSE_FRAMEWORK_DIR}/include/nodalkernels
    ${MOOSE_FRAMEWORK_DIR}/include/outputs
    ${MOOSE_FRAMEWORK_DIR}/include/parser
    ${MOOSE_FRAMEWORK_DIR}/include/partitioner
    ${MOOSE_FRAMEWORK_DIR}/include/postprocessors
    ${MOOSE_FRAMEWORK_DIR}/include/preconditioners
    ${MOOSE_FRAMEWORK_DIR}/include/predictors
    ${MOOSE_FRAMEWORK_DIR}/include/problems
    ${MOOSE_FRAMEWORK_DIR}/include/relationshipmanagers
    ${MOOSE_FRAMEWORK_DIR}/include/reporters
    ${MOOSE_FRAMEWORK_DIR}/include/restart
    ${MOOSE_FRAMEWORK_DIR}/include/samplers
    ${MOOSE_FRAMEWORK_DIR}/include/splits
    ${MOOSE_FRAMEWORK_DIR}/include/systems
    ${MOOSE_FRAMEWORK_DIR}/include/timeintegratros
    ${MOOSE_FRAMEWORK_DIR}/include/timesteppers
    ${MOOSE_FRAMEWORK_DIR}/include/transfers
    ${MOOSE_FRAMEWORK_DIR}/include/userobject
    ${MOOSE_FRAMEWORK_DIR}/include/utils
    ${MOOSE_FRAMEWORK_DIR}/include/variables
    ${MOOSE_FRAMEWORK_DIR}/include/vectorpostprocessors
    ${MOOSE_FRAMEWORK_DIR}/include/warehouse

    ${MOOSE_FRAMEWORK_DIR}/contrib/boost/include
    ${MOOSE_FRAMEWORK_DIR}/contrib/json/include
    ${MOOSE_FRAMEWORK_DIR}/contrib/hit
)

find_library(MOOSE_LIB moose-${METHOD} ${MOOSE_FRAMEWORK_DIR})
find_library(MOOSE_HIT_LIB hit-${METHOD} ${MOOSE_FRAMEWORK_DIR}/contrib/hit)

# godzilla
configure_file(include/base/GodzillaRevision.h.in GodzillaRevision.h)

set(GODZILLA_INCLUDE_DIRS
    include/base
    include/executioners
    include/input
    include/mesh
    include/problems
)

add_executable(godzilla-${METHOD}
    src/base/GodzillaApp.cpp
    src/base/GodzillaError.cpp
    src/executioners/GExecutioner.cpp
    src/input/GYMLParser.cpp
    src/mesh/GMesh.cpp
    src/problems/GPetscFeProblem.cpp
    #
    src/main.cpp
)

set_target_properties(godzilla-${METHOD} PROPERTIES UNITY_BUILD_MODE BATCH)

target_include_directories(godzilla-${METHOD} PRIVATE ${LIBMESH_INCLUDE_DIR})
target_include_directories(godzilla-${METHOD} PRIVATE ${MOOSE_INCLUDE_DIRS})
target_include_directories(godzilla-${METHOD} PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(godzilla-${METHOD} PRIVATE ${GODZILLA_INCLUDE_DIRS})

target_link_libraries(godzilla-${METHOD} PUBLIC yaml-cpp)
target_link_libraries(godzilla-${METHOD} PUBLIC ${LIBMESH_LIB})
target_link_libraries(godzilla-${METHOD} PUBLIC ${TIMPI_LIB})
target_link_libraries(godzilla-${METHOD} PUBLIC ${MOOSE_LIB})
target_link_libraries(godzilla-${METHOD} PUBLIC ${MOOSE_HIT_LIB})

# test
set(TEST_DIR ${PROJECT_SOURCE_DIR})

if(NOT PROCS EQUAL 0)
    set(N ${PROCS})
else()
    set(N 1)
endif()

add_custom_target(test)
add_custom_command(
  TARGET   test
  COMMAND  ${TEST_DIR}/run_tests -C ${TEST_DIR} -j${N}
)
