#include "gtest/gtest.h"
#include "Utils.h"

using namespace godzilla;

TEST(UtilsTest, to_lower)
{
    EXPECT_EQ(utils::to_lower("ASDF"), "asdf");
}

TEST(UtilsTest, to_upper)
{
    EXPECT_EQ(utils::to_upper("asdf"), "ASDF");
}
