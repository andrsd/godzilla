# Godzilla

add_library(godzilla
    SHARED
        # factory must be first
        Factory.cpp
        # then units with global variables
        RefMapShapeset1D.cpp
        H1LobattoShapesetEdge.cpp
        H1LobattoShapesetTetra.cpp
        QuadratureGauss1D.cpp
        QuadratureGauss2DTri.cpp
        QuadratureGauss3DTetra.cpp
        # and then the rest
        App.cpp
        CallStack.cpp
        CmdLineArgParser.cpp
        Edge.cpp
        Element.cpp
        Error.cpp
        ExodusIO.cpp
        GYMLFile.cpp
        Hex.cpp
        Init.cpp
        InputParameters.cpp
        Logger.cpp
        Mesh.cpp
        Object.cpp
        PrintInterface.cpp
        Quad.cpp
        RefDomain.cpp
        RefMap1D.cpp
        ShapeFunction1D.cpp
        Shapeset1D.cpp
        Shapeset2D.cpp
        Shapeset3D.cpp
        Terminal.cpp
        Tetra.cpp
        Tri.cpp
        Utils.cpp
        Vertex.cpp
)
target_code_coverage(godzilla)
target_sanitization(godzilla)

set_target_properties(
    godzilla
    PROPERTIES
        UNITY_BUILD_MODE BATCH
)

target_include_directories(
    godzilla
    PRIVATE
        ${CONDA_INCLUDE_DIR}
        ${HDF5_INCLUDE_DIR}
        ${JUDY_INCLUDE_DIR}
        ${PETSC_INCLUDE_DIR}
        ${PROJECT_BINARY_DIR}
        ${MUPARSER_INCLUDE_DIR}
        ${YAML_CPP_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/contrib
)

target_link_libraries(
    godzilla
    PUBLIC
        muparser
        yaml-cpp
        ${BLAS_LIBRARIES}
        ${JUDY_LIBRARY}
        ${HDF5_LIBRARY}
        ${NETCDF_LIBRARY}
        ${EXODUSII_LIBRARY}
        ${PETSC_LIBRARY}
)
