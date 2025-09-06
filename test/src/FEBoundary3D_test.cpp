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

class TestEssentialBoundary3D : public fe::EssentialBoundaryInfo<TET4, 3, 4> {
public:
    TestEssentialBoundary3D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<TET4, 3, 4>(mesh, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        EssentialBoundaryInfo::create();
        this->vals.create(this->num_vertices());
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        this->vals.destroy();
        EssentialBoundaryInfo::destroy();
    }

    void
    compute()
    {
        for_each_vertex([&](Int idx, Int vtx) { this->vals(idx) = vtx; });
    }

    Array1D<Int> vals;
};

class TestNaturalBoundary3D : public fe::NaturalBoundaryInfo<TET4, 3, 4> {
public:
    TestNaturalBoundary3D(UnstructuredMesh * mesh,
                          const Array1D<DenseMatrix<Real, 3, 4>> * grad_phi,
                          const IndexSet & facets) :
        fe::NaturalBoundaryInfo<TET4, 3, 4>(mesh, grad_phi, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        NaturalBoundaryInfo::create();
        this->vals.create(this->num_facets());
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        NaturalBoundaryInfo::destroy();
        this->vals.destroy();
    }

    void
    compute()
    {
        for_each_facet([&](Int idx, Int facet) { this->vals(idx) = facet; });
    }

    Array1D<Int> vals;
};

} // namespace

TEST(FEBoundaryTest, test_3d)
{
    TestApp app;

    auto mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<godzilla::App *>("_app", &app);
    TestMesh3D mesh(mesh_pars);
    mesh.create();

    constexpr Int DIM = 3;
    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto fe_volume = fe::calc_volumes<TET4, DIM>(*m);
    auto grad_phi = fe::calc_grad_shape<TET4, DIM>(*m, fe_volume);

    {
        auto label = m->get_label("left");
        auto bnd_facets = points_from_label(label);
        auto vertices = m->get_cone_recursive_vertices(bnd_facets);
        vertices.sort_remove_dups();

        TestEssentialBoundary3D bnd(m, vertices);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_vertices(), 3);

        EXPECT_EQ(bnd.vals(0), 1);

        bnd.destroy();
    }
    {
        auto label = m->get_label("front");
        auto bnd_facets = points_from_label(label);
        TestNaturalBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_facets(), 1);

        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);

        EXPECT_DOUBLE_EQ(bnd.facet_area(0), 0.5);

        EXPECT_EQ(bnd.vals(0), 6);

        bnd.destroy();
    }
    {
        auto label = m->get_label("bottom");
        auto bnd_facets = points_from_label(label);
        TestNaturalBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_facets(), 1);

        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), -1);

        EXPECT_DOUBLE_EQ(bnd.facet_area(0), 0.5);

        EXPECT_EQ(bnd.vals(0), 5);

        bnd.destroy();
    }
    {
        auto label = m->get_label("slanted");
        auto bnd_facets = points_from_label(label);
        TestNaturalBoundary3D bnd(m, &grad_phi, bnd_facets);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_facets(), 1);

        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0.57735026918962584);

        EXPECT_DOUBLE_EQ(bnd.facet_area(0), 0.5 * std::sqrt(3));

        EXPECT_EQ(bnd.vals(0), 8);

        bnd.destroy();
    }

    grad_phi.destroy();
    fe_volume.destroy();
}
