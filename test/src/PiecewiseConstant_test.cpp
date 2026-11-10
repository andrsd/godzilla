#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "godzilla/PiecewiseConstant.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(PiecewiseConstantTest, fn_parser_eval)
{
    PiecewiseConstant fn(Continuity::RIGHT, { 1., 2. }, { 3., 0., -1. });
    EXPECT_NEAR(fn.evaluate(0), 3., 1e-12);
}

TEST(PiecewiseConstantTest, left_cont_eval)
{
    PiecewiseConstant fn(Continuity::LEFT, { 1., 2., 3. }, { 3., 0., -1., 2. });
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
    PiecewiseConstant fn(Continuity::RIGHT, { 1., 2., 3. }, { 3., 0., -1., 2. });
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
    EXPECT_DEATH(PiecewiseConstant(Continuity::RIGHT, { 1., 2., 3. }, { 3., 1. }),
                 "'y' \\(size=2\\) must have one more entry than 'x' \\(size=3\\)");
}

TEST(PiecewiseConstantTest, err_no_points)
{
    EXPECT_DEATH(PiecewiseConstant(Continuity::RIGHT, {}, { 1. }),
                 "Size of 'x' is 0. It must be 1 or more");
}

TEST(PiecewiseConstantTest, err_not_monotonic)
{
    EXPECT_DEATH(PiecewiseConstant(Continuity::RIGHT, { 1., 0. }, { 1., 2., 3. }),
                 "Values in 'x' must be increasing. Failed at index '1'");
}

TEST(PiecewiseConstantTest, check_x_for_nans)
{
    std::vector<double> x = { 2.5, std::numeric_limits<double>::quiet_NaN(), 4.2 };
    EXPECT_DEATH(PiecewiseConstant ipol(Continuity::RIGHT, x, { 0., 2., 3., 4. }),
                 "Parameter 'x' contains a NaN");
}

TEST(PiecewiseConstantTest, check_y_for_nans)
{
    std::vector<double> y = { 1.0, 2.5, std::numeric_limits<double>::quiet_NaN(), 4.2 };
    EXPECT_DEATH(PiecewiseConstant ipol(Continuity::RIGHT, { 2., 3., 4. }, y),
                 "Parameter 'y' contains a NaN");
}

TEST(PiecewiseConstantTest, eval_uninitialized_object)
{
    PiecewiseConstant fn;
    EXPECT_DEATH(fn.evaluate(0.), "PiecewiseConstant is not initialized");
}

TEST(PiecewiseConstantTest, pass_into_parameters)
{
    Parameters params;
    params.add_param<PiecewiseConstant<Real>>("fn", "");

    PiecewiseConstant<Real> fn(Continuity::RIGHT, { 0.5 }, { -2, 3 });
    params.set<PiecewiseConstant<Real>>("fn", fn);

    auto par_fn = params.get<PiecewiseConstant<Real>>("fn");
    EXPECT_EQ(par_fn.get_continuity(), Continuity::RIGHT);
    EXPECT_NEAR(par_fn.evaluate(-10), -2, 1e-15);
    EXPECT_NEAR(par_fn.evaluate(0.5), 3, 1e-15);
    EXPECT_NEAR(par_fn.evaluate(10), 3, 1e-15);
}
