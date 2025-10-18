#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh2D.h"
#include "godzilla/Array1D.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/FEBoundary.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestEssentialBoundary2D : public fe::EssentialBoundaryInfo<TRI3, 2_D, 3> {
public:
    TestEssentialBoundary2D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<TRI3, 2_D, 3>(mesh, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        EssentialBoundaryInfo::create();
        this->vals = Array1D<Int>(this->num_vertices());
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        EssentialBoundaryInfo::destroy();
    }

    void
    compute()
    {
        for_each_vertex([&](Int idx, Int vtx) { this->vals(idx) = vtx; });
    }

    Array1D<Int> vals;
};

class TestNaturalBoundary2D : public fe::NaturalBoundaryInfo<TRI3, 2_D, 3> {
public:
    TestNaturalBoundary2D(UnstructuredMesh * mesh,
                          Array1D<DenseMatrix<Real, 2, 3>> grad_phi,
                          const IndexSet & facets) :
        fe::NaturalBoundaryInfo<TRI3, 2_D, 3>(mesh, grad_phi, facets)
    {
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        NaturalBoundaryInfo::create();
        this->vals = Array1D<Int>(this->num_facets());
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        NaturalBoundaryInfo::destroy();
    }

    void
    compute()
    {
        for_each_facet([&](Int idx, Int facet) { this->vals(idx) = facet; });
    }

    Array1D<Int> vals;
};

} // namespace

TEST(FEBoundaryTest, test_2d)
{
    TestApp app;

    auto mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("_app", &app);
    auto mesh_qtr = MeshFactory::create<TestMesh2D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    auto fe_volume = fe::calc_volumes<TRI3, 2>(*mesh);
    auto grad_phi = fe::calc_grad_shape<TRI3, 2>(*mesh, fe_volume);

    {
        auto label = mesh->get_label("left");
        auto bnd_facets = points_from_label(label);
        auto vertices = mesh->get_cone_recursive_vertices(bnd_facets);
        vertices.sort_remove_dups();
        TestEssentialBoundary2D bnd(mesh, vertices);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_vertices(), 2);

        EXPECT_EQ(bnd.vals(0), 2);
        EXPECT_EQ(bnd.vals(1), 4);

        bnd.destroy();
    }

    {
        auto label = mesh->get_label("bottom");
        auto bnd_facets = points_from_label(label);
        TestNaturalBoundary2D bnd(mesh, grad_phi, bnd_facets);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_facets(), 1);

        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);

        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);

        EXPECT_EQ(bnd.vals(0), 6);

        bnd.destroy();
    }

    {
        auto label = mesh->get_label("top_right");
        IndexSet bnd_facets = points_from_label(label);
        TestNaturalBoundary2D bnd(mesh, grad_phi, bnd_facets);
        bnd.create();
        bnd.compute();

        EXPECT_EQ(bnd.num_facets(), 2);

        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);

        EXPECT_DOUBLE_EQ(bnd.normal(1)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(1)(1), 1);

        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        EXPECT_DOUBLE_EQ(bnd.facet_length(1), 1.);

        EXPECT_EQ(bnd.vals(0), 9);
        EXPECT_EQ(bnd.vals(1), 10);

        bnd.destroy();
    }
}
