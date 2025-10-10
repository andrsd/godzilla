#include "gmock/gmock.h"
#include "godzilla/Math.h"

using namespace godzilla;

TEST(MathTest, sign)
{
    EXPECT_EQ(math::sign(-10.), -1.);
    EXPECT_EQ(math::sign(0.), 0.);
    EXPECT_EQ(math::sign(10.), 1.);
}

TEST(MathTest, sqr)
{
    EXPECT_DOUBLE_EQ(math::sqr(4.), 16.);
    EXPECT_EQ(math::sqr(4), 16);
}

TEST(MathTest, max)
{
    EXPECT_DOUBLE_EQ(math::max(4., 8.), 8.);
    EXPECT_DOUBLE_EQ(math::max({ -1, 10, 12, 20, -5, 0 }), 20);
}

TEST(MathTest, min)
{
    EXPECT_DOUBLE_EQ(math::min(4., 8.), 4.);
    EXPECT_DOUBLE_EQ(math::min({ -1, 10, 12, 20, -5, 0 }), -5);
}

TEST(MathTest, abs)
{
    EXPECT_DOUBLE_EQ(math::abs(4.), 4.);
    EXPECT_DOUBLE_EQ(math::abs(-8.), 8.);
}

TEST(MathTest, pow)
{
    EXPECT_DOUBLE_EQ(math::pow(4., 2), 16.);
    EXPECT_DOUBLE_EQ(math::pow<2>(7.), 49.);
    EXPECT_DOUBLE_EQ(math::pow(4., -1), 0.25);
}

TEST(MathTest, sqrt)
{
    EXPECT_DOUBLE_EQ(math::sqrt(16.), 4.);
}
