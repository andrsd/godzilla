#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/IndexSet.h"

using namespace godzilla;
using namespace testing;

TEST(IndexSetTest, create)
{
    TestApp app;
    IndexSet is;
    EXPECT_TRUE(is.is_null());
    is.create(app.get_comm());
    EXPECT_TRUE((IS) is != nullptr);
    EXPECT_TRUE(is);
    EXPECT_FALSE(is.is_null());
}

TEST(IndexSetTest, data)
{
    TestApp app;
    std::vector<Int> idxs = { 3, 5, 1, 8 };
    auto is = IndexSet::create_general(app.get_comm(), idxs, USE_POINTER);
    auto data = is.borrow_indices();
    EXPECT_THAT(data[0], 3);
    EXPECT_THAT(data[1], 5);
    EXPECT_THAT(data[2], 1);
    EXPECT_THAT(data[3], 8);
}

TEST(IndexSetTest, create_general)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 3, 5, 1, 8 });
    auto data = is.borrow_indices();
    auto idx = to_std_vector(data);
    EXPECT_THAT(idx, UnorderedElementsAre(1, 3, 5, 8));
}

TEST(IndexSetTest, get_id)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
    EXPECT_TRUE(is.get_id() != 0);
}

TEST(IndexSetTest, sort)
{
    TestApp app;
    IndexSet is = IndexSet::create_general(app.get_comm(), { 3, 5, 1, 8 });
    EXPECT_EQ(is.sorted(), false);
    is.sort();
    auto idx = is.borrow_indices();
    EXPECT_THAT(to_std_vector(idx), ElementsAreArray({ 1, 3, 5, 8 }));
    EXPECT_EQ(is.sorted(), true);
}

TEST(IndexSetTest, sort_remove_dups)
{
    TestApp app;
    IndexSet is = IndexSet::create_general(app.get_comm(), { 3, 1, 5, 3, 1, 8 });
    is.sort_remove_dups();
    auto idx = is.borrow_indices();
    EXPECT_THAT(to_std_vector(idx), ElementsAreArray({ 1, 3, 5, 8 }));
    EXPECT_EQ(is.sorted(), true);
}

TEST(IndexSetTest, intersect_caching)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 4, 5 });
    IndexSet isect = IndexSet::intersect_caching(is1, is2);
    EXPECT_EQ(isect.get_size(), 1);
    auto idx = isect.borrow_indices();
    EXPECT_EQ(idx[0], 3);
}

TEST(IndexSetTest, intersect_caching_empty)
{
    TestApp app;
    IndexSet is1;
    IndexSet is2;
    IndexSet isect = IndexSet::intersect_caching(is1, is2);
    EXPECT_FALSE(isect);
}

TEST(IndexSetTest, intersect)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 2, 3 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 4, 5 });
    IndexSet isect = IndexSet::intersect(is1, is2);
    EXPECT_EQ(isect.get_size(), 1);
    auto idx = isect.borrow_indices();
    EXPECT_EQ(idx[0], 3);
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

TEST(IndexSetTest, duplicate)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto dup = is.duplicate();
    auto vals = dup.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(1, 3, 4, 5, 8, 10));
}

TEST(IndexSetTest, shift)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    is.shift(2);
    auto vals = is.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(3, 5, 6, 7, 10, 12));
}

TEST(IndexSetTest, assign)
{
    TestApp app;
    auto src = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto dest = IndexSet::create_general(app.get_comm(), { 0, 0, 0, 0, 0, 0 });
    dest.assign(src);
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(1, 3, 4, 5, 8, 10));
}

TEST(IndexSetTest, copy)
{
    TestApp app;
    auto src = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto dest = IndexSet::create_general(app.get_comm(), { 0, 0, 0, 0, 0, 0 });
    IndexSet::copy(src, dest);
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(1, 3, 4, 5, 8, 10));
}

TEST(IndexSetTest, complement)
{
    TestApp app;
    auto src = IndexSet::create_general(app.get_comm(), { 4, 5, 6 });
    auto dest = src.complement(1, 10);
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(1, 2, 3, 7, 8, 9));
}

TEST(IndexSetTest, concatenate)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 2, 6, 7, 9 });
    auto dest = IndexSet::concatenate(app.get_comm(), { is1, is2 });
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(1, 3, 4, 5, 8, 10, 2, 6, 7, 9));
}

TEST(IndexSetTest, difference)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 5, 8 });
    auto dest = IndexSet::difference(is1, is2);
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ElementsAre(1, 4, 10));
}

TEST(IndexSetTest, equal)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto is3 = IndexSet::create_general(app.get_comm(), { 4, 5, 1, 8, 10, 3 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 5, 8 });
    EXPECT_TRUE(is1.equal(is3));
    EXPECT_FALSE(is1.equal(is2));
}

TEST(IndexSetTest, equal_unsorted)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto dup = is1.duplicate();
    auto is2 = IndexSet::create_general(app.get_comm(), { 4, 5, 1, 8, 10, 3 });
    EXPECT_TRUE(is1.equal_unsorted(dup));
    EXPECT_FALSE(is1.equal_unsorted(is2));
}

TEST(IndexSetTest, expand)
{
    TestApp app;
    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 5, 7, 9 });
    auto dest = IndexSet::expand(is1, is2);
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), UnorderedElementsAre(1, 3, 4, 5, 7, 8, 9, 10));
}

TEST(IndexSetTest, get_min_max)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto [min, max] = is.get_min_max();
    EXPECT_EQ(min, 1);
    EXPECT_EQ(max, 10);
}

TEST(IndexSetTest, get_type)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    EXPECT_EQ(is.get_type(), ISGENERAL);
}

TEST(IndexSetTest, identity)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    EXPECT_FALSE(is.identity());
    is.set_identity();
    EXPECT_TRUE(is.identity());
}

TEST(IndexSetTest, locate)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    EXPECT_EQ(is.locate(4), 2);
    EXPECT_TRUE(is.locate(7) < 0);
}

TEST(IndexSetTest, permutation)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    EXPECT_FALSE(is.permutation());
    is.set_permutation();
    EXPECT_TRUE(is.permutation());
}

TEST(IndexSetTest, set_type)
{
    TestApp app;
    IndexSet is;
    is.create(app.get_comm());
    is.set_type(ISGENERAL);
    EXPECT_EQ(is.get_type(), ISGENERAL);
}

TEST(IndexSetTest, sum)
{
    TestApp app;
    if (app.get_comm().size() != 1)
        return;

    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    auto is2 = IndexSet::create_general(app.get_comm(), { 3, 5, 7, 9 });
    auto dest = IndexSet::sum(is1, is2);
    auto vals = dest.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), UnorderedElementsAre(1, 3, 4, 5, 7, 8, 9, 10));
}

TEST(IndexSetTest, oper_bool)
{
    TestApp app;
    if (app.get_comm().size() != 1)
        return;

    IndexSet empty;
    const auto & cempty = empty;
    EXPECT_FALSE(empty);
    EXPECT_FALSE(cempty);

    auto is1 = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    const auto cis1 = &is1;
    EXPECT_TRUE(is1);
    EXPECT_TRUE(cis1);
}

TEST(IndexSetTest, range)
{
    TestApp app;
    auto is = IndexSet::create_general(app.get_comm(), { 1, 3, 4, 5, 8, 10 });
    std::vector<Int> idxs;
    auto vals = is.borrow_indices();
    for (auto & i : vals)
        idxs.push_back(i);
    EXPECT_THAT(idxs, ElementsAre(1, 3, 4, 5, 8, 10));
}

TEST(IndexSetTest, range_over_null_set)
{
    TestApp app;
    IndexSet is;
    is.create(app.get_comm());
    std::vector<Int> idxs;
    auto vals = is.borrow_indices();
    for (auto & i : vals)
        idxs.push_back(i);
    EXPECT_THAT(idxs, ElementsAre());
}
