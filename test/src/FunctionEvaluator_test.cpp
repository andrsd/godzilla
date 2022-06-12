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

TEST(FunctionEvaluatorTest, define_constant)
{
    FunctionEvaluator e;
    e.create("Re");
    e.define_constant("Re", 1234);
    PetscReal x[] = { 0. };
    PetscReal val = e.evaluate(1, 0., x);
    EXPECT_DOUBLE_EQ(1234., val);
}
