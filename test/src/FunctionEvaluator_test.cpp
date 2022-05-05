#include "gtest/gtest.h"
#include "FunctionEvaluator.h"
#include "Function.h"

using namespace godzilla;

TEST(FunctionEvaluatorTest, evaluate)
{
    FunctionEvaluator e;
    e.create("t*(x+y+z)");
    PetscReal x[] = { 3, 5, 8 };
    PetscReal val = e.evaluate(3, 2., x);
    EXPECT_EQ(val, 32.);
}

TEST(FunctionEvaluatorTest, nan)
{
    FunctionEvaluator e;
    e.create("a");
    PetscReal x[] = { 1 };
    PetscReal val = e.evaluate(1, 0., x);
    EXPECT_TRUE(!(val == val));
}
