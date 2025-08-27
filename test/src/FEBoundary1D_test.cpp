#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh1D.h"
#include "godzilla/MeshObject.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEBoundary.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBoundary1D : public fe::BoundaryInfo<EDGE2, 1, 2> {
public:
    TestBoundary1D(UnstructuredMesh * mesh,
                   const Array1D<DenseMatrix<Real, 1, 2>> * grad_phi,
                   const IndexSet & facets) :
        fe::BoundaryInfo<EDGE2, 1, 2>(mesh, grad_phi, facets)
    {
    }

    TestBoundary1D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::BoundaryInfo<EDGE2, 1, 2>(mesh, facets)
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

TEST(FEBoundaryTest, normals_1d)
{
    TestApp app;

    auto mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<godzilla::App *>("_app", &app);
    TestMesh1D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto fe_volume = fe::calc_volumes<EDGE2, 1>(*m);
    auto grad_phi = fe::calc_grad_shape<EDGE2, 1>(*m, fe_volume);

    {
        IndexSet bnd_facets = points_from_label(m->get_label("left"));
        TestBoundary1D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);
        bnd.destroy();
    }

    {
        IndexSet bnd_facets = points_from_label(m->get_label("right"));
        TestBoundary1D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.nodal_normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.length(0), 1.);
        bnd.destroy();
    }

    grad_phi.destroy();
    fe_volume.destroy();
}
