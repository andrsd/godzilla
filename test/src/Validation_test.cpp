#include "gtest/gtest.h"
#include "godzilla/Validation.h"

using namespace godzilla;

TEST(ValidationTest, in_const_char)
{
    EXPECT_TRUE(validation::in("a", { "q", "a", "z" }));
    EXPECT_FALSE(validation::in("p", { "q", "a", "z" }));
}

TEST(ValidationTest, in_str)
{
    String a = "a";
    EXPECT_TRUE(validation::in(a, { "q", "a", "z" }));
    String p = "p";
    EXPECT_FALSE(validation::in(p, { "q", "a", "z" }));
}

TEST(ValidationTest, in_int)
{
    EXPECT_TRUE(validation::in(1, { 1, 2, 3, 4 }));
    EXPECT_FALSE(validation::in(10, { 1, 2, 3, 4 }));
}

TEST(ValidationTest, in_range)
{
    EXPECT_TRUE(validation::in_range(1, { 0, 10 }));
    EXPECT_FALSE(validation::in_range(10, { -1, 1 }));

    EXPECT_TRUE(validation::in_range(1., { 0., 10. }));
    EXPECT_FALSE(validation::in_range(10., { -1, 1 }));
}
