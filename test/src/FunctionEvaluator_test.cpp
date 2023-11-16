#include "gtest/gtest.h"
#include "godzilla/FunctionEvaluator.h"
#include "godzilla/Function.h"

using namespace godzilla;

TEST(FunctionEvaluatorTest, evaluate)
{
    FunctionEvaluator e;
    e.create("t*(x+y+z)");
    Real x[] = { 3, 5, 8 };
    Real val = e.evaluate(3, 2., x);
    EXPECT_EQ(val, 32.);
}

TEST(FunctionEvaluatorTest, multi_evaluate)
{
    FunctionEvaluator e;
    e.create("t*(x^2+y^2),7890");
    Real x[] = { 3, 4 };
    Real val[2] = { 0., 0. };
    EXPECT_TRUE(e.evaluate(2, 3., x, 2, val));
    EXPECT_DOUBLE_EQ(75., val[0]);
    EXPECT_DOUBLE_EQ(7890., val[1]);
}

TEST(FunctionEvaluatorTest, error)
{
    FunctionEvaluator e;
    e.create("a");
    Real x[] = { 1 };
    EXPECT_DEATH(
        e.evaluate(1, 0., x),
        "\\[ERROR\\] Function evaluator failed: Unexpected token \"a\" found at position 0.");
}

TEST(FunctionEvaluatorTest, define_constant)
{
    FunctionEvaluator e;
    e.create("Re");
    e.define_constant("Re", 1234);
    Real x[] = { 0. };
    Real val = e.evaluate(1, 0., x);
    EXPECT_DOUBLE_EQ(1234., val);
}

TEST(FunctionEvaluatorTest, evaluate_error)
{
    FunctionEvaluator e;
    e.create("");
    Real x[] = { 0. };
    Real u[] = { 0. };
    EXPECT_DEATH(e.evaluate(1, 0., x, 1, u),
                 "\\[ERROR\\] Function evaluator failed: Expression is empty.");
}
