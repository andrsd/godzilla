include(TesterExodusII)

add_test_exodiff(
    NAME   heat-eqn-2d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/2d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/2d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/2d.exo
)

add_test_exodiff(
    NAME   heat-eqn-2d-explicit
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/2d-explicit.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/2d-explicit.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/2d-explicit.exo
)

add_test_exodiff(
    NAME   heat-eqn-mms-1d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/mms-1d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/mms-1d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/mms-1d.exo
    ARGS   -Floor 1e-15
)
