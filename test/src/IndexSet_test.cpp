#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/IndexSet.h"

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

TEST(IndexSetTest, data)
{
    TestApp app;
    IndexSet is = IndexSet::create_general(app.get_comm(), { 3, 5, 1, 8 });
    is.get_indices();
    auto data = is.data();
    EXPECT_THAT(data[0], 3);
    EXPECT_THAT(data[1], 5);
    EXPECT_THAT(data[2], 1);
    EXPECT_THAT(data[3], 8);
    is.restore_indices();
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
    auto is = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
    EXPECT_TRUE(is.get_id() != 0);
    is.destroy();
}

TEST(IndexSetTest, inc_ref)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
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
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 4, 5 });
    IndexSet isect = IndexSet::intersect_caching(is1, is2);
    EXPECT_EQ(isect.get_size(), 1);
    isect.get_indices();
    EXPECT_EQ(isect(0), 3);
    isect.restore_indices();
    is1.destroy();
    is2.destroy();
}

TEST(IndexSetTest, intersect_caching_empty)
{
    TestApp app;
    IndexSet is1;
    IndexSet is2;
    IndexSet isect = IndexSet::intersect_caching(is1, is2);
    EXPECT_TRUE(isect.empty());
}

TEST(IndexSetTest, intersect)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 4, 5 });
    IndexSet isect = IndexSet::intersect(is1, is2);
    EXPECT_EQ(isect.get_size(), 1);
    isect.get_indices();
    EXPECT_EQ(isect(0), 3);
    isect.restore_indices();
    is1.destroy();
    is2.destroy();
    isect.destroy();
}

TEST(IndexSetTest, range)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    is.get_indices();
    std::vector<Int> vals;
    for (auto & i : is)
        vals.push_back(i);
    EXPECT_THAT(vals, ElementsAre(1, 3, 4, 5, 8, 10));
    is.restore_indices();
}

TEST(IndexSetTest, for_loop)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    is.get_indices();
    std::vector<Int> vals;
    for (auto i = is.begin(); i != is.end(); i++)
        vals.push_back(*i);
    EXPECT_THAT(vals, ElementsAre(1, 3, 4, 5, 8, 10));
    is.restore_indices();
}

TEST(IndexSetTest, iters)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    is.get_indices();
    auto iter = is.begin();
    for (int i = 0; i < 6; i++, iter++)
        ;
    EXPECT_TRUE(iter == is.end());
    is.restore_indices();
}

TEST(IndexSetTest, view)
{
    testing::internal::CaptureStdout();
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 5, 8, 10 });
    is.view();
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("Number of indices in set 4"));
    EXPECT_THAT(out, HasSubstr("0 1"));
    EXPECT_THAT(out, HasSubstr("1 5"));
    EXPECT_THAT(out, HasSubstr("2 8"));
    EXPECT_THAT(out, HasSubstr("3 10"));
}
