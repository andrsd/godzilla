#include "gmock/gmock.h"
#include "Numerics.h"

using namespace godzilla;

TEST(NumericsTest, sign)
{
    EXPECT_EQ(math::sign(-10.), -1.);
    EXPECT_EQ(math::sign(0.), 0.);
    EXPECT_EQ(math::sign(10.), 1.);
}
