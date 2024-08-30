#include "gmock/gmock.h"
#include "godzilla/NDArray.h"

using namespace godzilla;

TEST(NDArrayTest, empty)
{
    NDArray<Real> arr;
    EXPECT_EQ(arr.size(), 0);
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.rank(), 0);
    EXPECT_EQ(arr.dimension().size(), 0);
}

TEST(NDArrayTest, rank_1)
{
    NDArray<Int> arr({ 5 });

    EXPECT_EQ(arr.size(), 5);
    EXPECT_FALSE(arr.empty());
    EXPECT_EQ(arr.rank(), 1);
    EXPECT_EQ(arr.dimension().size(), 1);
    EXPECT_EQ(arr.dimension()[0], 5);

    for (Int i = 0; i < 5; i++)
        arr(i) = i + 10;

    for (Int i = 0; i < 5; i++)
        EXPECT_EQ(arr(i), i + 10);
}

TEST(NDArrayTest, rank_1_init)
{
    NDArray<Real> arr({ 3 }, 2.);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_FALSE(arr.empty());
    EXPECT_EQ(arr.rank(), 1);
    EXPECT_EQ(arr.dimension().size(), 1);
    EXPECT_EQ(arr.dimension()[0], 3);
    for (Int i = 0; i < 3; i++)
        EXPECT_DOUBLE_EQ(arr(i), 2.);
}

TEST(NDArrayTest, rank_2)
{
    NDArray<Int> arr({ 5, 3 });

    EXPECT_EQ(arr.size(), 15);
    EXPECT_FALSE(arr.empty());
    EXPECT_EQ(arr.rank(), 2);
    EXPECT_EQ(arr.dimension().size(), 2);
    EXPECT_EQ(arr.dimension()[0], 5);
    EXPECT_EQ(arr.dimension()[1], 3);

    for (Int i = 0; i < 5; i++)
        for (Int j = 0; j < 3; j++)
            arr(i, j) = i * 100 + j;

    for (Int i = 0; i < 5; i++)
        for (Int j = 0; j < 3; j++)
            EXPECT_EQ(arr(i, j), i * 100 + j);
}

TEST(NDArrayTest, set)
{
    NDArray<Int> arr({ 3 });
    arr.set(123);
    for (Int i = 0; i < 3; i++)
        EXPECT_EQ(arr(i), 123.);
}
