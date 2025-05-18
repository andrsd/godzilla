#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh2D.h"
#include "godzilla/MeshObject.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEBoundary.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBoundary2D : public fe::BoundaryInfo<TRI3, 2, 3> {
public:
    TestBoundary2D(UnstructuredMesh * mesh,
                   const Array1D<DenseMatrix<Real, 2, 3>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<TRI3, 2, 3>(mesh, grad_phi, facets)
    {
    }

    TestBoundary2D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::BoundaryInfo<TRI3, 2, 3>(mesh, facets)
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

TEST(FEBoundaryTest, normals_2d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto fe_volume = fe::calc_volumes<TRI3, 2>(*m);
    auto grad_phi = fe::calc_grad_shape<TRI3, 2>(*m, fe_volume);

    {
        auto label = m->get_label("left");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);

        bnd.destroy();
    }

    {
        auto label = m->get_label("bottom");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);

        bnd.destroy();
    }

    {
        auto label = m->get_label("top_right");
        IndexSet bnd_facets = points_from_label(label);
        TestBoundary2D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(1)(1), 1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(1)(1), 1);

        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(0), 1. / std::sqrt(2));
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(2)(1), 1. / std::sqrt(2));

        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);
        EXPECT_DOUBLE_EQ(bnd.length(1), 1.);

        bnd.destroy();
    }
}
