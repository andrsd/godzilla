project(poisson-test)

include(TesterExodusII)

add_test_exodiff(
    NAME   poisson-fem-1d
    BIN    ${POISSON_BIN}
    INPUT  ${PROJECT_SOURCE_DIR}/gyml/fe_1d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/fe_1d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/gold/fe_1d.exo
)

add_test_exodiff(
    NAME   poisson-fem-2d
    BIN    ${POISSON_BIN}
    INPUT  ${PROJECT_SOURCE_DIR}/gyml/fe_2d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/fe_2d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/gold/fe_2d.exo
)

add_test_exodiff(
    NAME   poisson-fem-3d
    BIN    ${POISSON_BIN}
    INPUT  ${PROJECT_SOURCE_DIR}/gyml/fe_3d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/fe_3d.exo
    GOLD   ${PROJECT_SOURCE_DIR}/gold/fe_3d.exo
)
