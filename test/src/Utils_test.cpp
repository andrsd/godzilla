#include "gtest/gtest.h"
#include "Utils.h"

using namespace godzilla;

TEST(UtilsTest, to_lower)
{
    EXPECT_EQ(utils::toLower("ASDF"), "asdf");
}

TEST(UtilsTest, to_upper)
{
    EXPECT_EQ(utils::toUpper("asdf"), "ASDF");
}
