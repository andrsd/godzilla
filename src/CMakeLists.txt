# Godzilla

add_library(godzilla
    SHARED
        # factory must be first
        Factory.cpp
        App.cpp
        AuxiliaryField.cpp
        BoundaryCondition.cpp
        BoxMesh.cpp
        CallStack.cpp
        CmdLineArgParser.cpp
        ConstantIC.cpp
        CSVOutput.cpp
        DirichletBC.cpp
        Error.cpp
        EssentialBC.cpp
        ExodusIIMesh.cpp
        ExodusIIOutput.cpp
        ExplicitFELinearProblem.cpp
        FENonlinearProblem.cpp
        FEProblemInterface.cpp
        FileOutput.cpp
        Function.cpp
        FunctionAuxiliaryField.cpp
        FunctionEvaluator.cpp
        FunctionIC.cpp
        FunctionInterface.cpp
        GYMLFile.cpp
        HDF5Output.cpp
        ImplicitFENonlinearProblem.cpp
        Init.cpp
        InitialCondition.cpp
        InputParameters.cpp
        L2Diff.cpp
        LinearInterpolation.cpp
        LinearProblem.cpp
        LineMesh.cpp
        Logger.cpp
        Mesh.cpp
        MeshPartitioningOutput.cpp
        NaturalBC.cpp
        NonlinearProblem.cpp
        Object.cpp
        Output.cpp
        PiecewiseLinear.cpp
        Postprocessor.cpp
        PrintInterface.cpp
        Problem.cpp
        RectangleMesh.cpp
        Terminal.cpp
        TransientProblemInterface.cpp
        UnstructuredMesh.cpp
        Utils.cpp
        VTKOutput.cpp
)
target_code_coverage(godzilla)
target_sanitization(godzilla)

set_target_properties(
    godzilla
    PROPERTIES
        UNITY_BUILD_MODE BATCH
)

set_target_properties(
    godzilla
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(
    godzilla
    PRIVATE
        ${CONDA_INCLUDE_DIR}
        ${HDF5_INCLUDE_DIRS}
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
        Boost::filesystem
        ${NETCDF_LIBRARY}
        ${HDF5_LIBRARIES}
        ${EXODUSII_LIBRARY}
        ${PETSC_LIBRARY}
)
