#include "gmock/gmock.h"
#include "TestApp.h"
#include "IndexSet.h"
#include "RectangleMesh.h"

using namespace godzilla;
using namespace testing;

TEST(IndexSetTest, create)
{
    TestApp app;

    IndexSet is;
    EXPECT_TRUE(is.empty());
    is.create(app.get_comm());
    EXPECT_TRUE((IS) is != nullptr);
    is.destroy();
}

TEST(IndexSetTest, create_general)
{
    TestApp app;

    IndexSet is = IndexSet::create_general(app.get_comm(), { 3, 5, 1, 8 });
    is.get_indices();
    auto idx = is.to_std_vector();
    EXPECT_THAT(idx, UnorderedElementsAre(1, 3, 5, 8));
    is.restore_indices();
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

    auto label = mesh.get_label("Face Sets");
    auto is = label.get_values();

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

    auto label = mesh.get_label("Face Sets");
    auto is = label.get_values();

    is.inc_ref();

    Int cnt = 0;
    PetscObjectGetReference((PetscObject) (IS) is, &cnt);
    EXPECT_EQ(cnt, 2);

    is.destroy();
}

TEST(IndexSetTest, sort)
{
    TestApp app;

    IndexSet is = IndexSet::create_general(app.get_comm(), { 3, 5, 1, 8 });
    EXPECT_EQ(is.sorted(), false);
    is.sort();
    is.get_indices();
    auto idx = is.to_std_vector();
    EXPECT_THAT(idx, ElementsAreArray({ 1, 3, 5, 8 }));
    is.restore_indices();
    EXPECT_EQ(is.sorted(), true);
}

TEST(IndexSetTest, sort_remove_dups)
{
    TestApp app;

    IndexSet is = IndexSet::create_general(app.get_comm(), { 3, 1, 5, 3, 1, 8 });
    is.sort_remove_dups();
    is.get_indices();
    auto idx = is.to_std_vector();
    EXPECT_THAT(idx, ElementsAreArray({ 1, 3, 5, 8 }));
    is.restore_indices();
    EXPECT_EQ(is.sorted(), true);
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

    auto label1 = mesh.get_label("bottom");
    auto is1 = label1.get_values();
    auto label2 = mesh.get_label("right");
    auto is2 = label2.get_values();
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

    auto label1 = mesh.get_label("bottom");
    auto is1 = label1.get_values();
    auto label2 = mesh.get_label("right");
    auto is2 = label2.get_values();
    IndexSet isect = IndexSet::intersect(is1, is2);
    EXPECT_EQ(isect.get_size(), 0);
    is1.destroy();
    is2.destroy();
    isect.destroy();
}
