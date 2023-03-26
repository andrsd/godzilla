#include "gmock/gmock.h"
#include "TestApp.h"
#include "IndexSet.h"
#include "RectangleMesh.h"

using namespace godzilla;

TEST(IndexSetTest, create)
{
    TestApp app;

    IndexSet is;
    EXPECT_TRUE(is.empty());
    is.create(app.get_comm());
    EXPECT_TRUE((IS) is != nullptr);
    is.destroy();
}

TEST(IndexSetTest, values_from_label)
{
    TestApp app;

    Parameters params = RectangleMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 2;
    RectangleMesh mesh(params);
    mesh.create();

    DMLabel label = mesh.get_label("Face Sets");
    IndexSet is = IndexSet::values_from_label(label);
    EXPECT_EQ(is.get_size(), 4);
    EXPECT_EQ(is.get_local_size(), 4);

    is.get_indices();

    EXPECT_EQ(is[0], 4);
    EXPECT_EQ(is[1], 2);
    EXPECT_EQ(is[2], 1);
    EXPECT_EQ(is[3], 3);

    EXPECT_EQ(is(0), 4);
    EXPECT_EQ(is(1), 2);
    EXPECT_EQ(is(2), 1);
    EXPECT_EQ(is(3), 3);

    auto v = is.to_std_vector();
    EXPECT_THAT(v, testing::UnorderedElementsAre(1, 2, 3, 4));

    const Int * vals = is.data();
    EXPECT_EQ(vals[0], 4);
    EXPECT_EQ(vals[1], 2);
    EXPECT_EQ(vals[2], 1);
    EXPECT_EQ(vals[3], 3);

    is.restore_indices();

    is.destroy();
}

TEST(IndexSetTest, stratum_from_label)
{
    TestApp app;

    Parameters params = RectangleMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 2;
    RectangleMesh mesh(params);
    mesh.create();

    DMLabel label = mesh.get_label("celltype");
    IndexSet is = IndexSet::stratum_from_label(label, 0);
    EXPECT_EQ(is.get_size(), 9);
    is.destroy();
}

TEST(IndexSetTest, get_id)
{
    TestApp app;

    Parameters params = RectangleMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 2;
    RectangleMesh mesh(params);
    mesh.create();

    DMLabel label = mesh.get_label("Face Sets");
    IndexSet is = IndexSet::values_from_label(label);

    EXPECT_TRUE(is.get_id() != 0);

    is.destroy();
}

TEST(IndexSetTest, inc_ref)
{
    TestApp app;

    Parameters params = RectangleMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 2;
    RectangleMesh mesh(params);
    mesh.create();

    DMLabel label = mesh.get_label("Face Sets");
    IndexSet is = IndexSet::values_from_label(label);

    is.inc_ref();

    Int cnt = 0;
    PetscObjectGetReference((PetscObject) (IS) is, &cnt);
    EXPECT_EQ(cnt, 2);

    is.destroy();
}

TEST(IndexSetTest, intersect_caching)
{
    TestApp app;

    Parameters params = RectangleMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 2;
    RectangleMesh mesh(params);
    mesh.create();

    DMLabel label1 = mesh.get_label("1");
    IndexSet is1 = IndexSet::values_from_label(label1);
    DMLabel label2 = mesh.get_label("2");
    IndexSet is2 = IndexSet::values_from_label(label2);
    IndexSet isect = IndexSet::intersect_caching(is1, is2);
    EXPECT_EQ(isect.get_size(), 0);
    is1.destroy();
    is2.destroy();
}

TEST(IndexSetTest, intersect)
{
    TestApp app;

    Parameters params = RectangleMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 2;
    RectangleMesh mesh(params);
    mesh.create();

    DMLabel label1 = mesh.get_label("1");
    IndexSet is1 = IndexSet::values_from_label(label1);
    DMLabel label2 = mesh.get_label("2");
    IndexSet is2 = IndexSet::values_from_label(label2);
    IndexSet isect = IndexSet::intersect(is1, is2);
    EXPECT_EQ(isect.get_size(), 0);
    is1.destroy();
    is2.destroy();
    isect.destroy();
}
