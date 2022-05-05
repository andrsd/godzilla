project(poisson-test)

include(TesterHDF5)

add_test_hdf5diff(
    NAME   poisson-fem-1d
    BIN    ${POISSON_BIN}
    INPUT  ${PROJECT_SOURCE_DIR}/gyml/fe_1d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/fe_1d.h5
    GOLD   ${PROJECT_SOURCE_DIR}/gold/fe_1d.h5
)

add_test_hdf5diff(
    NAME   poisson-fem-2d
    BIN    ${POISSON_BIN}
    INPUT  ${PROJECT_SOURCE_DIR}/gyml/fe_2d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/fe_2d.h5
    GOLD   ${PROJECT_SOURCE_DIR}/gold/fe_2d.h5
)

add_test_hdf5diff(
    NAME   poisson-fem-3d
    BIN    ${POISSON_BIN}
    INPUT  ${PROJECT_SOURCE_DIR}/gyml/fe_3d.gyml
    OUTPUT ${PROJECT_BINARY_DIR}/fe_3d.h5
    GOLD   ${PROJECT_SOURCE_DIR}/gold/fe_3d.h5
)
