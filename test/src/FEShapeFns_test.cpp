#include "gmock/gmock.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"

using namespace godzilla;

TEST(FEShapeFns, grad_shape_egde2)
{
    DenseVector<DenseVector<Real, 1>, 2> coords;
    coords(0) = DenseVector<Real, 1>({ 1. });
    coords(1) = DenseVector<Real, 1>({ 3. });
    Real volume = fe::volume<EDGE2>(coords);
    auto grads = fe::grad_shape<EDGE2, 1>(coords, volume);
    EXPECT_DOUBLE_EQ(grads(0, 0), -0.5);
    EXPECT_DOUBLE_EQ(grads(1, 0), 0.5);
}

TEST(FEShapeFns, grad_shape_tri3)
{
    DenseVector<DenseVector<Real, 2>, 3> coords;
    coords(0) = DenseVector<Real, 2>({ 0, 0. });
    coords(1) = DenseVector<Real, 2>({ 1, 0. });
    coords(2) = DenseVector<Real, 2>({ 0, 1. });
    Real volume = fe::volume<TRI3>(coords);
    auto grads = fe::grad_shape<TRI3, 2>(coords, volume);
    EXPECT_DOUBLE_EQ(grads(0, 0), -1);
    EXPECT_DOUBLE_EQ(grads(0, 1), -1);
    EXPECT_DOUBLE_EQ(grads(1, 0), 1);
    EXPECT_DOUBLE_EQ(grads(1, 1), 0);
    EXPECT_DOUBLE_EQ(grads(2, 0), 0);
    EXPECT_DOUBLE_EQ(grads(2, 1), 1);
}

TEST(FEShapeFns, calc_grad_shape)
{
    const ElementType ELEM_TYPE = TRI3;
    const int DIM = 2;
    const int n_elems = 2;
    const int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE);
    Array1D<DenseVector<Real, DIM>> coords;
    coords.create(4);
    coords(0) = DenseVector<Real, DIM>({ 0, 0. });
    coords(1) = DenseVector<Real, DIM>({ 1, 0. });
    coords(2) = DenseVector<Real, DIM>({ 0, 1. });
    coords(3) = DenseVector<Real, DIM>({ 1, 1. });
    Array1D<DenseVector<Int, N_ELEM_NODES>> connect;
    connect.create(n_elems);
    connect(0) = DenseVector<Int, N_ELEM_NODES>({ 0, 1, 2 });
    connect(1) = DenseVector<Int, N_ELEM_NODES>({ 1, 3, 2 });
    Array1D<Real> volumes;
    volumes.create(n_elems);
    fe::calc_volumes<ELEM_TYPE, DIM>(coords, connect, volumes);
    auto grad_sh = fe::calc_grad_shape<ELEM_TYPE, DIM>(coords, connect, volumes);
    grad_sh.destroy();
    volumes.destroy();
    connect.destroy();
    coords.destroy();
}
