#include "gtest/gtest.h"
#include "ParsedFunctionEvaluator.h"

using namespace godzilla;

TEST(ParsedFunctionEvaluatorTest, evaluate)
{
    ParsedFunctionEvaluator e;
    e.create("t*(x+y+z)");
    Real val = e.evaluate(3, 2., 3, 5, 8);
    EXPECT_EQ(val, 32.);
}

TEST(ParsedFunctionEvaluatorTest, nan)
{
    ParsedFunctionEvaluator e;
    e.create("a");
    Real val = e.evaluate(1, 0., 1., 0, 0);
    EXPECT_TRUE(!(val == val));
}
