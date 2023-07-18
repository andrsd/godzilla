#include "gmock/gmock.h"
#include "FEGeometry.h"
#include "FEShapeFns.h"

using namespace godzilla;
using namespace testing;

TEST(FEGeometryTest, common_elements_by_node)
{
    Array1D<DenseVector<Int, 2>> connect(3);
    connect(0) = DenseVector<Int, 2>({ 0, 1 });
    connect(1) = DenseVector<Int, 2>({ 1, 2 });
    connect(2) = DenseVector<Int, 2>({ 2, 3 });

    Array1D<std::vector<Int>> nelcom;
    nelcom.create(4);
    fe::common_elements_by_node(connect, nelcom);

    EXPECT_THAT(nelcom(0), UnorderedElementsAre(0));
    EXPECT_THAT(nelcom(1), UnorderedElementsAre(0, 1));
    EXPECT_THAT(nelcom(2), UnorderedElementsAre(1, 2));
    EXPECT_THAT(nelcom(3), UnorderedElementsAre(2));

    nelcom.destroy();
    connect.destroy();
}

TEST(FEGeometryTest, normal_edge2)
{
    Real volume = 2.;
    DenseVector<DenseVector<Real, 1>, 2> coords;
    coords(0) = DenseVector<Real, 1>({ 0 });
    coords(1) = DenseVector<Real, 1>({ 2 });
    auto grad = fe::grad_shape<EDGE2, 1>(coords, volume);
    {
        auto n = fe::normal<EDGE2>(volume, 0, grad(1));
        EXPECT_DOUBLE_EQ(n(0), -1);
    }
    {
        auto n = fe::normal<EDGE2>(volume, 0., grad(0));
        EXPECT_DOUBLE_EQ(n(0), 1);
    }
}

TEST(FEGeometryTest, normal_tri3)
{
    Real volume = 0.5;
    DenseVector<DenseVector<Real, 2>, 3> coords;
    coords(0) = DenseVector<Real, 2>({ 0, 0 });
    coords(1) = DenseVector<Real, 2>({ 1, 0 });
    coords(2) = DenseVector<Real, 2>({ 0, 1 });
    auto grad = fe::grad_shape<TRI3, 2>(coords, volume);
    {
        Real edge_len = 1.;
        auto n = fe::normal<TRI3>(volume, edge_len, grad(1));
        EXPECT_DOUBLE_EQ(n(0), -1.);
        EXPECT_DOUBLE_EQ(n(1), 0.);
    }
    {
        Real edge_len = 1.;
        auto n = fe::normal<TRI3>(volume, edge_len, grad(2));
        EXPECT_DOUBLE_EQ(n(0), 0.);
        EXPECT_DOUBLE_EQ(n(1), -1.);
    }
    {
        Real edge_len = std::sqrt(2.);
        auto n = fe::normal<TRI3>(volume, edge_len, grad(0));
        EXPECT_DOUBLE_EQ(n(0), 1. / std::sqrt(2.));
        EXPECT_DOUBLE_EQ(n(1), 1. / std::sqrt(2.));
    }
}

TEST(FEGeometryTest, element_length_edge2)
{
    Real volume = 2.;
    DenseVector<DenseVector<Real, 1>, 2> coords;
    coords(0) = DenseVector<Real, 1>({ 0 });
    coords(1) = DenseVector<Real, 1>({ 2 });
    auto grad = fe::grad_shape<EDGE2, 1>(coords, volume);
    auto len = fe::element_length<EDGE2, 1, 2>(grad);
    EXPECT_DOUBLE_EQ(len, 2.);
}

TEST(FEGeometryTest, element_length_tri3)
{
    Real volume = 0.5;
    DenseVector<DenseVector<Real, 2>, 3> coords;
    coords(0) = DenseVector<Real, 2>({ 0, 0 });
    coords(1) = DenseVector<Real, 2>({ 1, 0 });
    coords(2) = DenseVector<Real, 2>({ 0, 1 });
    auto grad = fe::grad_shape<TRI3, 2>(coords, volume);
    auto len = fe::element_length<TRI3, 2, 3>(grad);
    EXPECT_DOUBLE_EQ(len, 1. / std::sqrt(2.));
}

TEST(FEGeometryTest, calc_nodal_radius_xyz_edge2)
{
    Array1D<DenseVector<Real, 1>> coords;
    coords.create(2);
    coords(0) = DenseVector<Real, 1>({ 0. });
    coords(1) = DenseVector<Real, 1>({ 2. });
    auto rad = fe::calc_nodal_radius<CARTESIAN, 1>(coords);
    EXPECT_DOUBLE_EQ(rad(0), 1.);
    EXPECT_DOUBLE_EQ(rad(1), 1.);
}

TEST(FEGeometryTest, calc_nodal_radius_xyz_tri3)
{
    Array1D<DenseVector<Real, 2>> coords;
    coords.create(3);
    coords(0) = DenseVector<Real, 2>({ 0., 0. });
    coords(1) = DenseVector<Real, 2>({ 1., 0. });
    coords(2) = DenseVector<Real, 2>({ 0., 1. });
    auto rad = fe::calc_nodal_radius<CARTESIAN, 2>(coords);
    EXPECT_DOUBLE_EQ(rad(0), 1.);
    EXPECT_DOUBLE_EQ(rad(1), 1.);
    EXPECT_DOUBLE_EQ(rad(2), 1.);
}

TEST(FEGeometryTest, calc_nodal_radius_xyz_tet4)
{
    Array1D<DenseVector<Real, 3>> coords;
    coords.create(4);
    coords(0) = DenseVector<Real, 3>({ 0., 0., 0. });
    coords(1) = DenseVector<Real, 3>({ 1., 0., 0. });
    coords(2) = DenseVector<Real, 3>({ 0., 1., 0. });
    coords(3) = DenseVector<Real, 3>({ 0., 0., 1. });
    auto rad = fe::calc_nodal_radius<CARTESIAN, 3>(coords);
    EXPECT_DOUBLE_EQ(rad(0), 1.);
    EXPECT_DOUBLE_EQ(rad(1), 1.);
    EXPECT_DOUBLE_EQ(rad(2), 1.);
    EXPECT_DOUBLE_EQ(rad(3), 1.);
}

TEST(FEGeometryTest, calc_nodal_radius_rz_tri3)
{
    Array1D<DenseVector<Real, 2>> coords;
    coords.create(3);
    coords(0) = DenseVector<Real, 2>({ 0., 0. });
    coords(1) = DenseVector<Real, 2>({ 1., 0. });
    coords(2) = DenseVector<Real, 2>({ 0., 1. });
    auto rad = fe::calc_nodal_radius<godzilla::AXISYMMETRIC, 2>(coords);
    EXPECT_DOUBLE_EQ(rad(0), 0.);
    EXPECT_DOUBLE_EQ(rad(1), 0.);
    EXPECT_DOUBLE_EQ(rad(2), 1.);
}

TEST(FEGeometryTest, get_local_face_index)
{
    std::vector<Int> elem = { 2, 4, 7 };
    EXPECT_EQ(fe::get_local_face_index(elem, { 2, 4 }), 2);
    EXPECT_EQ(fe::get_local_face_index(elem, { 4, 7 }), 0);
    EXPECT_EQ(fe::get_local_face_index(elem, { 7, 2 }), 1);
}
