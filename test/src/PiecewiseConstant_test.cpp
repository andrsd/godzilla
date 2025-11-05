#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "godzilla/PiecewiseConstant.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(PiecewiseConstantTest, fn_parser_eval)
{
    PiecewiseConstant fn(PiecewiseConstant::RIGHT, { 1., 2. }, { 3., 0., -1. });
    EXPECT_NEAR(fn.evaluate(0), 3., 1e-12);
}

TEST(PiecewiseConstantTest, left_cont_eval)
{
    PiecewiseConstant fn(PiecewiseConstant::LEFT, { 1., 2., 3. }, { 3., 0., -1., 2. });
    EXPECT_DOUBLE_EQ(fn.evaluate(0.), 3.);
    EXPECT_DOUBLE_EQ(fn.evaluate(1.), 3.);
    EXPECT_DOUBLE_EQ(fn.evaluate(1.5), 0.);
    EXPECT_DOUBLE_EQ(fn.evaluate(2.), 0.);
    EXPECT_DOUBLE_EQ(fn.evaluate(2.5), -1.);
    EXPECT_DOUBLE_EQ(fn.evaluate(3.), -1.);
    EXPECT_DOUBLE_EQ(fn.evaluate(3.5), 2.);
}

TEST(PiecewiseConstantTest, right_cont_eval)
{
    PiecewiseConstant fn(PiecewiseConstant::RIGHT, { 1., 2., 3. }, { 3., 0., -1., 2. });
    EXPECT_DOUBLE_EQ(fn.evaluate(0.), 3.);
    EXPECT_DOUBLE_EQ(fn.evaluate(1.), 0.);
    EXPECT_DOUBLE_EQ(fn.evaluate(1.5), 0.);
    EXPECT_DOUBLE_EQ(fn.evaluate(2.), -1.);
    EXPECT_DOUBLE_EQ(fn.evaluate(2.5), -1.);
    EXPECT_DOUBLE_EQ(fn.evaluate(3.), 2.);
    EXPECT_DOUBLE_EQ(fn.evaluate(3.5), 2.);
}

TEST(PiecewiseConstantTest, err_incorrect_point_count)
{
    EXPECT_DEATH(PiecewiseConstant(PiecewiseConstant::RIGHT, { 1., 2. }, { 3., 1. }),
                 "'x' \\(size=2\\) must have one more entry than 'y' \\(size=2\\)");
}

TEST(PiecewiseConstantTest, err_no_points)
{
    EXPECT_DEATH(PiecewiseConstant(PiecewiseConstant::RIGHT, {}, { 1. }),
                 "Size of 'x' is 0. It must be 1 or more");
}

TEST(PiecewiseConstantTest, err_not_monotonic)
{
    EXPECT_THROW_MSG(PiecewiseConstant(PiecewiseConstant::RIGHT, { 1., 0. }, { 1., 2., 3. }),
                     "Values in 'x' must be increasing - failed at index '1'");
}

TEST(PiecewiseConstantTest, pass_into_parameters)
{
    PiecewiseConstant fn(PiecewiseConstant::RIGHT, { 0.5 }, { -2, 3 });

    Parameters params;
    params.set<PiecewiseConstant>("fn", fn);

    auto par_fn = params.get<PiecewiseConstant>("fn");
    EXPECT_EQ(par_fn.get_continuity(), PiecewiseConstant::RIGHT);
    EXPECT_NEAR(par_fn.evaluate(-10), -2, 1e-15);
    EXPECT_NEAR(par_fn.evaluate(0.5), 3, 1e-15);
    EXPECT_NEAR(par_fn.evaluate(10), 3, 1e-15);
}
