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

class TestBoundary1D : public fe::EssentialBoundaryInfo<EDGE2, 1, 2> {
public:
    TestBoundary1D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<EDGE2, 1, 2>(mesh, facets)
    {
    }
};

class TestBoundary2D : public fe::EssentialBoundaryInfo<TRI3, 2, 3> {
public:
    TestBoundary2D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<TRI3, 2, 3>(mesh, facets)
    {
    }
};

class TestBoundary3D : public fe::EssentialBoundaryInfo<TET4, 3, 4> {
public:
    TestBoundary3D(UnstructuredMesh * mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<TET4, 3, 4>(mesh, facets)
    {
    }
};

} // namespace

TEST(EssentialBoundaryTest, test_1d)
{
    TestApp app;

    auto mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh1D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    {
        auto bnd_facets = points_from_label(m->get_label("left"));
        TestBoundary1D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 1);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 2);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(m->get_label("right"));
        TestBoundary1D bnd(m, bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 1);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 4);
        bnd.destroy();
    }
}

TEST(EssentialBoundaryTest, test_2d)
{
    TestApp app;

    auto mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    {
        auto bnd_facets = points_from_label(m->get_label("left"));
        auto vtxs = boundary_vertices(m, bnd_facets);
        TestBoundary2D bnd(m, vtxs);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 2);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 2);
        EXPECT_DOUBLE_EQ(bnd.vertex(1), 4);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(m->get_label("bottom"));
        auto vtxs = boundary_vertices(m, bnd_facets);
        TestBoundary2D bnd(m, vtxs);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 2);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 2);
        EXPECT_DOUBLE_EQ(bnd.vertex(1), 3);
        bnd.destroy();
    }
}

TEST(EssentialBoundaryTest, test_3d)
{
    TestApp app;

    Parameters mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh3D mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    {
        auto label = m->get_label("left");
        auto bnd_facets = points_from_label(label);
        auto vtxs = boundary_vertices(m, bnd_facets);
        TestBoundary3D bnd(m, vtxs);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 3);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 1);
        EXPECT_DOUBLE_EQ(bnd.vertex(1), 3);
        EXPECT_DOUBLE_EQ(bnd.vertex(2), 4);
        bnd.destroy();
    }
}
