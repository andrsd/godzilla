#include "gmock/gmock.h"
#include "godzilla/FEVolumes.h"
#include "ExceptionTestMacros.h"
#include "godzilla/Types.h"

using namespace godzilla;

TEST(FEVolumesTest, volume_edge2)
{
    DenseMatrix<Real, 2, 1> coords;
    coords.set_row(0, { 1. });
    coords.set_row(1, { 3. });
    EXPECT_EQ(fe::volume<EDGE2>(coords), 2.);
}

TEST(FEVolumesTest, volume_edge2_2d)
{
    DenseMatrix<Real, 2, 2> coords;
    coords.set_row(0, { 4., 9. });
    coords.set_row(1, { 1., 5. });
    EXPECT_EQ(fe::volume<EDGE2>(coords), 5.);
}

TEST(FEVolumesTest, volume_tri3)
{
    DenseMatrix<Real, 3, 2> coords;
    coords.set_row(0, { 0., 0. });
    coords.set_row(1, { 1., 0. });
    coords.set_row(2, { 0., 1. });
    EXPECT_EQ(fe::volume<TRI3>(coords), 0.5);
}

TEST(FEVolumesTest, volume_tet4)
{
    DenseMatrix<Real, 4, 3> coords;
    coords.set_row(0, { 0., 0., 0 });
    coords.set_row(1, { 1., 0., 0 });
    coords.set_row(2, { 0., 1., 0. });
    coords.set_row(3, { 0., 0., 1. });
    EXPECT_EQ(fe::volume<TET4>(coords), 1. / 6.);
}

TEST(FEVolumesTest, calc_volumes)
{
    const Int N_NODES = 5;
    const Int N_ELEMS = 4;
    Array1D<DenseVector<Real, 2>> coords(N_NODES);
    Array1D<DenseVector<Int, 3>> connect(N_ELEMS);
    Array1D<godzilla::Real> fe_volume(N_ELEMS);

    coords(0) = DenseVector<Real, 2>({ 0, 0 });
    coords(1) = DenseVector<Real, 2>({ 1, 0 });
    coords(2) = DenseVector<Real, 2>({ 1, 1 });
    coords(3) = DenseVector<Real, 2>({ 0, 1 });
    coords(4) = DenseVector<Real, 2>({ 0.4, 0.2 });

    connect(0) = DenseVector<Int, 3>({ 0, 1, 4 });
    connect(1) = DenseVector<Int, 3>({ 1, 2, 4 });
    connect(2) = DenseVector<Int, 3>({ 2, 3, 4 });
    connect(3) = DenseVector<Int, 3>({ 3, 0, 4 });

    fe::calc_volumes<TRI3>(coords, connect, fe_volume);

    EXPECT_DOUBLE_EQ(fe_volume(0), 0.1);
    EXPECT_DOUBLE_EQ(fe_volume(1), 0.3);
    EXPECT_DOUBLE_EQ(fe_volume(2), 0.4);
    EXPECT_DOUBLE_EQ(fe_volume(3), 0.2);
}

TEST(FEVolumesTest, calc_volumes_1d_petsc)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    constexpr Dimension DIM = 1_D;
    const Int N_ELEM_NODES = 2;
    std::vector<Int> cells = { 0, 1, 1, 2, 2, 3 };
    std::vector<Real> coords = { 0, 1, 1.5, 3 };
    auto mesh =
        UnstructuredMesh::build_from_cell_list(comm, DIM, N_ELEM_NODES, cells, DIM, coords, true);

    auto volumes = fe::calc_volumes<EDGE2, 1>(*mesh);

    EXPECT_DOUBLE_EQ(volumes(0), 1.);
    EXPECT_DOUBLE_EQ(volumes(1), 0.5);
    EXPECT_DOUBLE_EQ(volumes(2), 1.5);
}

TEST(FEVolumesTest, calc_volumes_2d_petsc)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    const Dimension DIM = 2_D;
    const Int N_ELEM_NODES = 3;
    std::vector<Int> cells = { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 };
    std::vector<Real> coords = { 0, 0, 1, 0, 1, 1, 0, 1, 0.4, 0.2 };
    auto mesh =
        UnstructuredMesh::build_from_cell_list(comm, DIM, N_ELEM_NODES, cells, DIM, coords, true);

    auto volumes = fe::calc_volumes<TRI3, 2>(*mesh);

    EXPECT_DOUBLE_EQ(volumes(0), 0.1);
    EXPECT_DOUBLE_EQ(volumes(1), 0.3);
    EXPECT_DOUBLE_EQ(volumes(2), 0.4);
    EXPECT_DOUBLE_EQ(volumes(3), 0.2);
}

TEST(FEVolumesTest, face_area_edge2)
{
    DenseMatrix<Real, 1, 1> coords;
    coords(0, 0) = 0.;
    auto A = fe::face_area<EDGE2>(coords);
    EXPECT_DOUBLE_EQ(A, 1.);
}

TEST(FEVolumesTest, face_area_tri3)
{
    DenseMatrix<Real, 2, 2> coords;
    coords.set_row(0, { 0., 0. });
    coords.set_row(1, { 1., 1. });
    auto A = fe::face_area<TRI3>(coords);
    EXPECT_DOUBLE_EQ(A, std::sqrt(2));
}

TEST(FEVolumesTest, face_area_tet4)
{
    DenseMatrix<Real, 3, 3> coords;
    coords.set_row(0, { 0., 0., 0. });
    coords.set_row(1, { 1., 0., 0. });
    coords.set_row(2, { 0., 1., 0. });
    EXPECT_THROW_MSG(fe::face_area<TET4>(coords),
                     "Face area calculation for TET4 in 3 dimensions is not implemented");
}
