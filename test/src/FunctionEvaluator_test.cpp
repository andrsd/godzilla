#include "gtest/gtest.h"
#include "FunctionEvaluator.h"
#include "Function.h"

using namespace godzilla;

TEST(FunctionEvaluatorTest, evaluate)
{
    FunctionEvaluator e;
    const std::vector<Function *> storage;
    e.create("t*(x+y+z)", storage);
    PetscReal x[] = { 3, 5, 8 };
    PetscReal val = e.evaluate(3, 2., x);
    EXPECT_EQ(val, 32.);
}

TEST(FunctionEvaluatorTest, nan)
{
    FunctionEvaluator e;
    const std::vector<Function *> storage;
    e.create("a", storage);
    PetscReal x[] = { 1 };
    PetscReal val = e.evaluate(1, 0., x);
    EXPECT_TRUE(!(val == val));
}
