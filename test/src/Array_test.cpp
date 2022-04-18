#include "gtest/gtest.h"
#include "Array.h"

using namespace godzilla;

TEST(ArrayTest, set)
{
    Array<int> arr;
    arr.set(0, 1234);

    EXPECT_TRUE(arr.exists(0));
    EXPECT_EQ(arr[0], 1234);
    EXPECT_EQ(arr.get(0), 1234);
}

TEST(ArrayTest, add)
{
    Array<int> arr;
    arr.add(12);
    arr.add(34);

    EXPECT_EQ(arr[0], 12);
    EXPECT_EQ(arr[1], 34);
    EXPECT_EQ(arr.count(), 2);
}

TEST(ArrayTest, remove)
{
    Array<int> arr;
    arr[0] = 12;
    arr[1] = 34;

    arr.remove(0);

    EXPECT_FALSE(arr.exists(0));
    EXPECT_TRUE(arr.exists(1));
    EXPECT_EQ(arr[1], 34);
    EXPECT_EQ(arr.count(), 1);
}

TEST(ArrayTest, remove_all)
{
    Array<int> arr;
    arr[0] = 12;
    arr[1] = 34;

    arr.remove_all();

    EXPECT_EQ(arr.count(), 0);
}

TEST(ArrayTest, iters)
{
    Array<int> arr;
    arr[0] = 12;
    arr[2] = 56;
    arr[4] = 90;

    EXPECT_EQ(arr.first(), 0);
    EXPECT_EQ(arr.next(0), 2);
    EXPECT_EQ(arr.prev(4), 2);
    EXPECT_EQ(arr.last(), 4);
}
