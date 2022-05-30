include(TesterExodusII)

add_test_exodiff(
    NAME   burgers
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/burgers.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/burgers.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/burgers.exo
    ARGS   -Floor 1e-16
)
