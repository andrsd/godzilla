include(TesterExodusII)

if(${PETSC_VERSION} VERSION_LESS "3.21.0")

add_test_exodiff(
    NAME   advect
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/1d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/1d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/1d.exo
    ARGS   -Floor 1e-16
)

endif()