#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh3D.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/MeshObject.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEBoundary.h"
#include "godzilla/Types.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBoundary3D : public fe::BoundaryInfo<TET4, 3, 4> {
public:
    TestBoundary3D(UnstructuredMesh * mesh,
                   const Array1D<DenseMatrix<Real, 3, 4>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<TET4, 3, 4>(mesh, grad_phi, facets)
    {
    }

    TestBoundary3D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::BoundaryInfo<TET4, 3, 4>(mesh, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        compute_face_normals();
        create_vertex_index_set();
        compute_nodal_normals();
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        free();
    }
};

} // namespace

TEST(FEBoundaryTest, normals_3d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh3D mesh(mesh_pars);
    mesh.create();

    constexpr Int DIM = 3;
    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto fe_volume = fe::calc_volumes<TET4, DIM>(*m);
    auto grad_phi = fe::calc_grad_shape<TET4, DIM>(*m, fe_volume);

    {
        auto label = m->get_label("left");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(2), 0);

        EXPECT_DOUBLE_EQ(bnd.length(0), 0.5);
        bnd.destroy();
    }
    {
        auto label = m->get_label("front");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(2), 0);

        EXPECT_DOUBLE_EQ(bnd.area(0), 0.5);
        bnd.destroy();
    }
    {
        auto label = m->get_label("bottom");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(2), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(2), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(2), -1);

        EXPECT_DOUBLE_EQ(bnd.area(0), 0.5);
        bnd.destroy();
    }
    {
        auto label = m->get_label("slanted");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0.57735026918962584);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(2), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(2), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(2), 0.57735026918962584);

        EXPECT_DOUBLE_EQ(bnd.area(0), 0.5 * std::sqrt(3));
        bnd.destroy();
    }

    grad_phi.destroy();
    fe_volume.destroy();
}
