# Godzilla

add_library(godzilla
    SHARED
        # factory must be first
        Factory.cpp
        App.cpp
        CallStack.cpp
        CmdLineArgParser.cpp
        GYMLFile.cpp
        Init.cpp
        InputParameters.cpp
        Logger.cpp
        Object.cpp
        PrintInterface.cpp
        Terminal.cpp
        Utils.cpp
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
        ${PETSC_LIBRARY}
)
