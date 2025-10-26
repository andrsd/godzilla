#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh1D.h"
#include "TestMesh2D.h"
#include "TestMesh3D.h"
#include "godzilla/Enums.h"
#include "godzilla/FEBoundary.h"
#include "godzilla/MeshFactory.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBoundary1D : public fe::NaturalBoundaryInfo<EDGE2, 1_D, 2> {
public:
    TestBoundary1D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<EDGE2, 1_D, 2>(mesh, facets)
    {
    }
};

class TestBoundary2D : public fe::NaturalBoundaryInfo<TRI3, 2_D, 3> {
public:
    TestBoundary2D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<TRI3, 2_D, 3>(mesh, facets)
    {
    }
};

class TestBoundary3D : public fe::NaturalBoundaryInfo<TET4, 3_D, 4> {
public:
    TestBoundary3D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<TET4, 3_D, 4>(mesh, facets)
    {
    }
};

} // namespace

TEST(NaturalBoundaryTest, test_1d)
{
    TestApp app;

    auto mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    auto mesh_qtr = MeshFactory::create<TestMesh1D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        TestBoundary1D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 2);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(mesh->get_label("right"));
        TestBoundary1D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 4);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        bnd.destroy();
    }
}

TEST(NaturalBoundaryTest, test_2d)
{
    TestApp app;

    auto mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    auto mesh_qtr = MeshFactory::create<TestMesh2D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        TestBoundary2D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 8);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(mesh->get_label("bottom"));
        TestBoundary2D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 6);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        bnd.destroy();
    }
}

TEST(NaturalBoundaryTest, test_3d)
{
    TestApp app;

    auto mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    auto mesh_qtr = MeshFactory::create<TestMesh3D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    {
        auto label = mesh->get_label("left");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 8);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        bnd.destroy();
    }

    {
        auto label = mesh->get_label("bottom");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 5);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        bnd.destroy();
    }

    {
        auto label = mesh->get_label("front");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 6);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        bnd.destroy();
    }

    {
        auto label = mesh->get_label("slanted");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(mesh, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 7);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5 * std::sqrt(3));
        bnd.destroy();
    }
}
