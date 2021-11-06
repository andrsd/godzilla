#include "gtest/gtest.h"
#include "Utils.h"

using namespace godzilla;

TEST(Utils_test, to_lower)
{
    EXPECT_EQ(
        utils::toLower("ASDF"),
        "asdf"
    );
}

TEST(Utils_test, to_upper)
{
    EXPECT_EQ(
        utils::toUpper("asdf"),
        "ASDF"
    );
}
