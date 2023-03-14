#include "gmock/gmock.h"
#include "Array2D.h"

using namespace godzilla;

TEST(Array2DTest, create)
{
    Array2D<Real> arr;
    arr.create(2, 5);
    EXPECT_EQ(arr.get_num_rows(), 2);
    EXPECT_EQ(arr.get_num_cols(), 5);
    arr.destroy();
}

TEST(Array2DTest, zero)
{
    Array2D<Real> arr;
    arr.create(2, 5);
    for (Int i = 0; i < 2; i++)
        for (Int j = 0; j < 5; j++)
            arr(i, j) = 1.;
    arr.zero();
    for (Int i = 0; i < 2; i++)
        for (Int j = 0; j < 5; j++)
            EXPECT_EQ(arr(i, j), 0.);
    arr.destroy();
}
