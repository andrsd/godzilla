add_library(${PROJECT_NAME}
    SHARED
        # factory must be first
        Factory.cpp
        App.cpp
        AuxiliaryField.cpp
        BasicTSAdapt.cpp
        BndJacobianFunc.cpp
        BndResidualFunc.cpp
        BoundaryCondition.cpp
        BoxMesh.cpp
        CallStack.cpp
        ConstantAuxiliaryField.cpp
        ConstantInitialCondition.cpp
        CSVOutput.cpp
        DependencyEvaluator.cpp
        DirichletBC.cpp
        DiscreteProblemInterface.cpp
        Error.cpp
        EssentialBC.cpp
        ExodusIIMesh.cpp
        ExodusIIOutput.cpp
        ExplicitFELinearProblem.cpp
        ExplicitFVLinearProblem.cpp
        FENonlinearProblem.cpp
        FEProblemInterface.cpp
        FileMesh.cpp
        FileOutput.cpp
        Function.cpp
        Functional.cpp
        ValueFunctional.cpp
        FunctionAuxiliaryField.cpp
        FunctionEvaluator.cpp
        FunctionInitialCondition.cpp
        FunctionInterface.cpp
        FVProblemInterface.cpp
        GmshMesh.cpp
        GYMLFile.cpp
        ImplicitFENonlinearProblem.cpp
        IndexSet.cpp
        Init.cpp
        InitialCondition.cpp
        InputFile.cpp
        JacobianFunc.cpp
        L2Diff.cpp
        L2FieldDiff.cpp
        Label.cpp
        LinearInterpolation.cpp
        LinearProblem.cpp
        LineMesh.cpp
        Logger.cpp
        Matrix.cpp
        Mesh.cpp
        MeshPartitioningOutput.cpp
        NaturalBC.cpp
        NaturalRiemannBC.cpp
        NonlinearProblem.cpp
        Object.cpp
        Output.cpp
        Parameters.cpp
        ParsedFunction.cpp
        Partitioner.cpp
        PerfLog.cpp
        PetscFEGodzilla.cpp
        PiecewiseConstant.cpp
        PiecewiseLinear.cpp
        Postprocessor.cpp
        PrintInterface.cpp
        Problem.cpp
        Quadrature.cpp
        Random.cpp
        RectangleMesh.cpp
        ResidualFunc.cpp
        Section.cpp
        TecplotOutput.cpp
        Terminal.cpp
        TimeSteppingAdaptor.cpp
        TransientProblemInterface.cpp
        Types.cpp
        UnstructuredMesh.cpp
        Utils.cpp
        Validation.cpp
        ValueFunctional.cpp
        Vector.cpp
        VTKOutput.cpp
        WeakForm.cpp
)
target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        UNITY_BUILD_MODE BATCH
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<INSTALL_INTERFACE:include/godzilla>
    PRIVATE
        ${PETSC_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        fmt::fmt
        yaml-cpp
    PRIVATE
        ${PETSC_LIBRARY}
        muParser
        exodusIIcpp::exodusIIcpp
)
if (GODZILLA_WITH_MPI)
    target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_CXX)
endif()


set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_RPATH OFF)
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# install

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/GodzillaConfig.cmake.in
    GodzillaConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
    NO_SET_AND_CHECK_MACRO
)
write_basic_package_version_file(
    GodzillaConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS godzilla
    EXPORT GodzillaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING PATTERN "*.h"
)

install(
    FILES ${CMAKE_BINARY_DIR}/GodzillaConfig.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/cxxopts
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    EXPORT GodzillaTargets
    FILE GodzillaTargets.cmake
    NAMESPACE Godzilla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/GodzillaConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/GodzillaConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
)
