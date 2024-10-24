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
    std::string a = "a";
    EXPECT_TRUE(validation::in(a, { "q", "a", "z" }));
    std::string p = "p";
    EXPECT_FALSE(validation::in(p, { "q", "a", "z" }));
}

TEST(ValidationTest, in_int)
{
    EXPECT_TRUE(validation::in(1, { 1, 2, 3, 4 }));
    EXPECT_FALSE(validation::in(10, { 1, 2, 3, 4 }));
}
