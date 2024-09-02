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
