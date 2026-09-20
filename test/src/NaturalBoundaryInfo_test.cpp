#include "gmock/gmock.h"
#include "TestApp.h"
#include "TestMesh1D.h"
#include "TestMesh2D.h"
#include "TestMesh3D.h"
#include "godzilla/Enums.h"
#include "godzilla/FEBoundary.h"
#include "godzilla/MeshFactory.h"

using namespace godzilla;

namespace {

class TestBoundary1D : public fe::NaturalBoundaryInfo<EDGE2, 1_D, 2> {
public:
    TestBoundary1D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<EDGE2, 1_D, 2>(mesh, facets)
    {
    }
};

class TestBoundary2D : public fe::NaturalBoundaryInfo<TRI3, 2_D, 3> {
public:
    TestBoundary2D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<TRI3, 2_D, 3>(mesh, facets)
    {
    }
};

class TestBoundary3D : public fe::NaturalBoundaryInfo<TET4, 3_D, 4> {
public:
    TestBoundary3D(Ref<UnstructuredMesh> mesh, const IndexSet & facets) :
        fe::NaturalBoundaryInfo<TET4, 3_D, 4>(mesh, facets)
    {
    }
};

} // namespace

TEST(NaturalBoundaryTest, test_1d)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<TestMesh1D>();
    auto mesh_qtr = MeshFactory::create<TestMesh1D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        TestBoundary1D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 2);
    }

    {
        auto bnd_facets = points_from_label(mesh->get_label("right"));
        TestBoundary1D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 4);
    }
}

TEST(NaturalBoundaryTest, test_2d)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<TestMesh2D>();
    auto mesh_qtr = MeshFactory::create<TestMesh2D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    {
        auto bnd_facets = points_from_label(mesh->get_label("left"));
        TestBoundary2D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 8);
    }

    {
        auto bnd_facets = points_from_label(mesh->get_label("bottom"));
        TestBoundary2D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 1.);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 6);
    }
}

TEST(NaturalBoundaryTest, test_3d)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<TestMesh3D>();
    auto mesh_qtr = MeshFactory::create<TestMesh3D>(mesh_pars);
    auto mesh = mesh_qtr.get();

    {
        auto label = mesh->get_label("left");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 8);
    }

    {
        auto label = mesh->get_label("bottom");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), -1);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 5);
    }

    {
        auto label = mesh->get_label("front");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), -1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5);
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 6);
    }

    {
        auto label = mesh->get_label("slanted");
        auto bnd_facets = points_from_label(label);
        TestBoundary3D bnd(ref(*mesh), bnd_facets);
        EXPECT_DOUBLE_EQ(bnd.num_facets(), 1);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(0), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(1), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.normal(0)(2), 0.57735026918962584);
        EXPECT_DOUBLE_EQ(bnd.facet_length(0), 0.5 * std::sqrt(3));
        auto facets = bnd.facets().borrow_indices();
        EXPECT_DOUBLE_EQ(facets[0], 7);
    }
}
