#include "gmock/gmock.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEVolumes.h"
#include "ExceptionTestMacros.h"
#include "mpicpp-lite/mpicpp-lite.h"

using namespace godzilla;
using namespace testing;
namespace mpi = mpicpp_lite;

TEST(FEGeometryTest, coordinates)
{
    mpi::Communicator comm;
    UnstructuredMesh mesh = *UnstructuredMesh::build_from_cell_list(comm,
                                                                    1,
                                                                    2,
                                                                    { 0, 1, 1, 2, 2, 3 },
                                                                    1,
                                                                    { 0., 0.1, 0.2, 0.3 },
                                                                    true);

    auto coords = fe::coordinates<1>(mesh);
    ASSERT_EQ(coords.get_size(), 4);
    EXPECT_DOUBLE_EQ(coords(3)(0), 0.);
    EXPECT_DOUBLE_EQ(coords(4)(0), 0.1);
    EXPECT_DOUBLE_EQ(coords(5)(0), 0.2);
    EXPECT_DOUBLE_EQ(coords(6)(0), 0.3);
}

TEST(FEGeometryTest, connectivity)
{
    mpi::Communicator comm;
    UnstructuredMesh mesh = *UnstructuredMesh::build_from_cell_list(comm,
                                                                    1,
                                                                    2,
                                                                    { 0, 1, 1, 2, 2, 3 },
                                                                    1,
                                                                    { 0., 0.1, 0.2, 0.3 },
                                                                    true);
    auto connect = fe::connectivity<1, 2>(mesh);
    ASSERT_EQ(connect.get_size(), 3);
    EXPECT_EQ(connect(0)(0), 3);
    EXPECT_EQ(connect(0)(1), 4);
    EXPECT_EQ(connect(1)(0), 4);
    EXPECT_EQ(connect(1)(1), 5);
    EXPECT_EQ(connect(2)(0), 5);
    EXPECT_EQ(connect(2)(1), 6);
}

TEST(FEGeometryTest, common_elements_by_node)
{
    mpi::Communicator comm;
    UnstructuredMesh mesh = *UnstructuredMesh::build_from_cell_list(comm,
                                                                    1,
                                                                    2,
                                                                    { 0, 1, 1, 2, 2, 3 },
                                                                    1,
                                                                    { 0., 0.1, 0.2, 0.3 },
                                                                    true);

    auto nelcom = fe::common_elements_by_node<2>(mesh);

    EXPECT_THAT(nelcom(0), UnorderedElementsAre(0));
    EXPECT_THAT(nelcom(1), UnorderedElementsAre(0, 1));
    EXPECT_THAT(nelcom(2), UnorderedElementsAre(1, 2));
    EXPECT_THAT(nelcom(3), UnorderedElementsAre(2));

    nelcom.destroy();
}

TEST(FEGeometryTest, normal_edge2)
{
    Real volume = 2.;
    DenseMatrix<Real, 2, 1> coords;
    coords.set_row(0, { 0 });
    coords.set_row(1, { 2 });
    auto grad = fe::grad_shape<EDGE2, 1>(coords, volume);
    {
        auto n = fe::normal<EDGE2>(volume, 0, DenseVector<Real, 1>(grad.column(1)));
        EXPECT_DOUBLE_EQ(n(0), -1);
    }
    {
        auto n = fe::normal<EDGE2>(volume, 0., DenseVector<Real, 1>(grad.column(0)));
        EXPECT_DOUBLE_EQ(n(0), 1);
    }
}

TEST(FEGeometryTest, normal_tri3)
{
    Real volume = 0.5;
    DenseMatrix<Real, 3, 2> coords;
    coords.set_row(0, { 0, 0 });
    coords.set_row(1, { 1, 0 });
    coords.set_row(2, { 0, 1 });
    auto grad = fe::grad_shape<TRI3, 2>(coords, volume);
    {
        Real edge_len = 1.;
        auto n = fe::normal<TRI3>(volume, edge_len, DenseVector<Real, 2>(grad.column(1)));
        EXPECT_DOUBLE_EQ(n(0), -1.);
        EXPECT_DOUBLE_EQ(n(1), 0.);
    }
    {
        Real edge_len = 1.;
        auto n = fe::normal<TRI3>(volume, edge_len, DenseVector<Real, 2>(grad.column(2)));
        EXPECT_DOUBLE_EQ(n(0), 0.);
        EXPECT_DOUBLE_EQ(n(1), -1.);
    }
    {
        Real edge_len = std::sqrt(2.);
        auto n = fe::normal<TRI3>(volume, edge_len, DenseVector<Real, 2>(grad.column(0)));
        EXPECT_DOUBLE_EQ(n(0), 1. / std::sqrt(2.));
        EXPECT_DOUBLE_EQ(n(1), 1. / std::sqrt(2.));
    }
}

TEST(FEGeometryTest, normal_hex8)
{
    DenseVector<Real, 10> grad;
    EXPECT_THROW_MSG(
        (fe::normal<EDGE2, 10>(1., 1., grad)),
        "Computation of a normal for element 'EDGE2' in 10 dimensions is not implemented");
}

TEST(FEGeometryTest, element_length_edge2)
{
    Real volume = 2.;
    DenseMatrix<Real, 2, 1> coords;
    coords.set_row(0, { 0 });
    coords.set_row(1, { 2 });
    auto grad = fe::grad_shape<EDGE2, 1>(coords, volume);
    auto len = fe::element_length<EDGE2, 1, 2>(grad);
    EXPECT_DOUBLE_EQ(len, 2.);
}

TEST(FEGeometryTest, element_length_tri3)
{
    Real volume = 0.5;
    DenseMatrix<Real, 3, 2> coords;
    coords.set_row(0, { 0, 0 });
    coords.set_row(1, { 1, 0 });
    coords.set_row(2, { 0, 1 });
    auto grad = fe::grad_shape<TRI3, 2>(coords, volume);
    auto len = fe::element_length<TRI3, 2, 3>(grad);
    EXPECT_DOUBLE_EQ(len, 1. / std::sqrt(2.));
}

TEST(FEGeometryTest, calc_element_length)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    const ElementType ELEM_TYPE = TRI3;
    const int DIM = 2;
    const int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE);

    std::vector<Int> cells = { 0, 1, 2, 2, 1, 3 };
    std::vector<Real> coords = { 0, 0, 0.8, 0., 0., 0.6, 1, 0.6 };
    auto mesh =
        UnstructuredMesh::build_from_cell_list(comm, DIM, N_ELEM_NODES, cells, DIM, coords, true);
    auto volumes = fe::calc_volumes<ELEM_TYPE, DIM>(*mesh);
    auto grad_sh = fe::calc_grad_shape<ELEM_TYPE, DIM>(*mesh, volumes);
    auto hel = fe::calc_element_length<ELEM_TYPE, DIM>(grad_sh);
    EXPECT_DOUBLE_EQ(hel(0), 0.48);
    EXPECT_DOUBLE_EQ(hel(1), 0.6);
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

TEST(FEGeometryTest, get_local_vertex_index)
{
    std::vector<Int> elem = { 2, 7, 4 };
    EXPECT_EQ(fe::get_local_vertex_index(elem, 2), 0);
    EXPECT_EQ(fe::get_local_vertex_index(elem, 7), 1);
    EXPECT_EQ(fe::get_local_vertex_index(elem, 4), 2);
    EXPECT_THROW_MSG(fe::get_local_vertex_index(elem, 0),
                     "Vertex 0 is not part of the connectivity array.");
}

TEST(FEGeometryTest, get_local_face_index)
{
    std::vector<Int> elem = { 2, 4, 7 };
    EXPECT_EQ(fe::get_local_face_index(elem, { 2, 4 }), 2);
    EXPECT_EQ(fe::get_local_face_index(elem, { 4, 7 }), 0);
    EXPECT_EQ(fe::get_local_face_index(elem, { 7, 2 }), 1);
}
