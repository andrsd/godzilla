#include "gmock/gmock.h"
#include "godzilla/Range.h"

using namespace godzilla;

TEST(RangeTest, make_range)
{
    std::vector<Int> arr;
    for (auto & i : make_range(5))
        arr.push_back(i);
    EXPECT_THAT(arr, testing::ElementsAre(0, 1, 2, 3, 4));
}

TEST(RangeTest, make_range_2)
{
    std::vector<Int> arr;
    for (auto & i : make_range(4, 9))
        arr.push_back(i);
    EXPECT_THAT(arr, testing::ElementsAre(4, 5, 6, 7, 8));
}

TEST(RangeTest, oper_plus)
{
    auto rng = make_range(2, 7);
    auto n = rng.first() + 2;
    EXPECT_EQ(n, 4);
}

TEST(RangeTest, oper_plus_n)
{
    auto rng = make_range(2, 7);
    auto n = rng.first();
    n += 3;
    EXPECT_EQ(n, 5);
}

TEST(RangeTest, oper_minus)
{
    auto rng = make_range(2, 7);
    EXPECT_EQ(rng.last() - rng.first(), 5);
}

TEST(RangeTest, oper_minus_n)
{
    auto rng = make_range(2, 7);
    auto n = rng.last() - 3;
    EXPECT_EQ(n, 4);
}

TEST(RangeTest, oper_less_than)
{
    auto rng = make_range(5);
    EXPECT_TRUE(rng.first() < rng.last());
    EXPECT_FALSE(rng.last() < rng.first());
}

TEST(RangeTest, contains)
{
    auto rng = make_range(2, 7);
    EXPECT_FALSE(rng.contains(1));
    EXPECT_TRUE(rng.contains(2));
    EXPECT_TRUE(rng.contains(6));
    EXPECT_FALSE(rng.contains(7));
}
