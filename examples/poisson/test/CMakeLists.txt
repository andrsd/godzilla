include(TesterExodusII)

add_test_exodiff(
    NAME   poisson-fem-1d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/fe_1d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/fe_1d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/fe_1d.exo
)

add_test_exodiff(
    NAME   poisson-fem-2d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/fe_2d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/fe_2d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/fe_2d.exo
)

add_test_exodiff(
    NAME   poisson-fem-3d
    BIN    ${PROJECT_NAME}
    INPUT  ${PROJECT_SOURCE_DIR}/test/gyml/fe_3d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/test/fe_3d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/test/gold/fe_3d.exo
)
