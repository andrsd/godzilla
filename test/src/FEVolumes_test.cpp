#include "gmock/gmock.h"
#include "FEVolumes.h"

using namespace godzilla;

TEST(FEVolumesTest, volume_edge2)
{
    DenseVector<DenseVector<Real, 1>, 2> coords;
    coords(0) = DenseVector<Real, 1>({ 1. });
    coords(1) = DenseVector<Real, 1>({ 3. });
    EXPECT_EQ(fe::volume<EDGE2>(coords), 2.);
}

TEST(FEVolumesTest, volume_edge2_2d)
{
    DenseVector<DenseVector<Real, 2>, 2> coords;
    coords(0) = DenseVector<Real, 2>({ 4., 9. });
    coords(1) = DenseVector<Real, 2>({ 1., 5. });
    EXPECT_EQ(fe::volume<EDGE2>(coords), 5.);
}

TEST(FEVolumesTest, volume_tri3)
{
    DenseVector<DenseVector<Real, 2>, 3> coords;
    coords(0) = DenseVector<Real, 2>({ 0., 0. });
    coords(1) = DenseVector<Real, 2>({ 1., 0. });
    coords(2) = DenseVector<Real, 2>({ 0., 1. });
    EXPECT_EQ(fe::volume<TRI3>(coords), 0.5);
}

TEST(FEVolumesTest, volume_tet4)
{
    DenseVector<DenseVector<Real, 3>, 4> coords;
    coords(0) = DenseVector<Real, 3>({ 0., 0., 0 });
    coords(1) = DenseVector<Real, 3>({ 1., 0., 0 });
    coords(2) = DenseVector<Real, 3>({ 0., 1., 0. });
    coords(3) = DenseVector<Real, 3>({ 0., 0., 1. });
    EXPECT_DEATH(fe::volume<TET4>(coords),
                 "Volume calculation for TET4 in 3 dimensions is not implemented");
}

TEST(FEVolumesTest, calc_volumes)
{
    const Int N_NODES = 5;
    const Int N_ELEMS = 4;
    Array1D<DenseVector<Real, 2>> coords;
    Array1D<DenseVector<Int, 3>> connect;
    Array1D<godzilla::Real> fe_volume;
    coords.create(N_NODES);
    connect.create(N_ELEMS);
    fe_volume.create(N_ELEMS);

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

    coords.destroy();
    connect.destroy();
    fe_volume.destroy();
}
