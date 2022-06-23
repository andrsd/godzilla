include(TesterExodusII)

add_test_exodiff(
    NAME   poisson-mms-1d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/mms-1d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/mms-1d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/mms-1d.exo
)

add_test_exodiff(
    NAME   poisson-mms-2d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/mms-2d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/mms-2d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/mms-2d.exo
)

add_test_exodiff(
    NAME   poisson-mms-3d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/mms-3d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/mms-3d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/mms-3d.exo
)
