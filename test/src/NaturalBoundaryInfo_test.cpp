#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh1D.h"
#include "TestMesh2D.h"
#include "TestMesh3D.h"
#include "godzilla/Enums.h"
#include "godzilla/MeshObject.h"
#include "godzilla/FEBoundary.h"

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
    mesh_pars.set<godzilla::App *>("_app", &app);
    TestMesh1D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    {
        auto bnd_facets = points_from_label(m->get_label("left"));
        TestBoundary1D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 2);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(m->get_label("right"));
        TestBoundary1D bnd(m, bnd_facets);
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
    mesh_pars.set<godzilla::App *>("_app", &app);
    TestMesh2D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    {
        auto bnd_facets = points_from_label(m->get_label("left"));
        TestBoundary2D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 8);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(m->get_label("bottom"));
        TestBoundary2D bnd(m, bnd_facets);
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
    mesh_pars.set<godzilla::App *>("_app", &app);
    TestMesh3D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    {
        auto label = m->get_label("left");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.facet(0), 7);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        bnd.destroy();
    }
}
