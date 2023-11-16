#include "gtest/gtest.h"
#include "godzilla/Validation.h"

using namespace godzilla;

TEST(ValidationTest, in)
{
    EXPECT_TRUE(validation::in("a", { "q", "a", "z" }));
    EXPECT_FALSE(validation::in("p", { "q", "a", "z" }));
}
