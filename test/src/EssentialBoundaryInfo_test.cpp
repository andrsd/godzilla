#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh1D.h"
#include "TestMesh2D.h"
#include "TestMesh3D.h"
#include "godzilla/Enums.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/FEBoundary.h"

using namespace godzilla;

namespace {

class TestBoundary1D : public fe::EssentialBoundaryInfo<EDGE2, 1_D, 2> {
public:
    TestBoundary1D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<EDGE2, 1_D, 2>(mesh, facets)
    {
    }
};

class TestBoundary2D : public fe::EssentialBoundaryInfo<TRI3, 2_D, 3> {
public:
    TestBoundary2D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<TRI3, 2_D, 3>(mesh, facets)
    {
    }
};

class TestBoundary3D : public fe::EssentialBoundaryInfo<TET4, 3_D, 4> {
public:
    TestBoundary3D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::EssentialBoundaryInfo<TET4, 3_D, 4>(mesh, facets)
    {
    }
};

} // namespace

TEST(EssentialBoundaryTest, test_1d)
{
    TestApp app;

    auto mesh_pars = TestMesh1D::parameters();
    mesh_pars.set<Ref<godzilla::App>>("app", ref(app));
    auto mesh = MeshFactory::create<TestMesh1D>(mesh_pars);

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        TestBoundary1D bnd(ref(*mesh), bnd_facets);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 1);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 2);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(mesh->get_label("right"));
        TestBoundary1D bnd(ref(*mesh), bnd_facets);
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
    mesh_pars.set<Ref<godzilla::App>>("app", ref(app));
    auto mesh = MeshFactory::create<TestMesh2D>(mesh_pars);

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        auto vtxs = boundary_vertices(mesh.get(), bnd_facets);
        TestBoundary2D bnd(ref(*mesh), vtxs);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 2);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 2);
        EXPECT_DOUBLE_EQ(bnd.vertex(1), 4);
        bnd.destroy();
    }

    {
        auto bnd_facets = points_from_label(mesh->get_label("bottom"));
        auto vtxs = boundary_vertices(mesh.get(), bnd_facets);
        TestBoundary2D bnd(ref(*mesh), vtxs);
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

    auto mesh_pars = TestMesh3D::parameters();
    mesh_pars.set<Ref<godzilla::App>>("app", ref(app));
    auto mesh = MeshFactory::create<TestMesh3D>(mesh_pars);

    {
        auto label = mesh->get_label("left");
        auto bnd_facets = points_from_label(label);
        auto vtxs = boundary_vertices(mesh.get(), bnd_facets);
        TestBoundary3D bnd(ref(*mesh), vtxs);
        bnd.create();
        EXPECT_DOUBLE_EQ(bnd.num_vertices(), 3);
        EXPECT_DOUBLE_EQ(bnd.vertex(0), 1);
        EXPECT_DOUBLE_EQ(bnd.vertex(1), 3);
        EXPECT_DOUBLE_EQ(bnd.vertex(2), 4);
        bnd.destroy();
    }
}
