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
