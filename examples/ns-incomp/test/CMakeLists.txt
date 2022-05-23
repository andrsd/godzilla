include(TesterExodusII)

add_test_exodiff(
    NAME   ${PROJECT_NAME}-mms-2d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/mms-2d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/mms-2d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/mms-2d.exo
    ARGS   -Floor 1e-12
)
