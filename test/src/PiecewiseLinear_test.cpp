#include "gmock/gmock.h"
#include "godzilla/PiecewiseLinear.h"
#include "godzilla/Exception.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(PiecewiseLinearTest, sample)
{
    std::vector<Real> x = { 1, 2, 3 };
    std::vector<Real> y = { 0, 4, 2 };
    PiecewiseLinear lipol(x, y);

    EXPECT_EQ(lipol.evaluate(0.0), 0.);
    EXPECT_EQ(lipol.evaluate(1.0), 0.);
    EXPECT_EQ(lipol.evaluate(1.5), 2.);
    EXPECT_EQ(lipol.evaluate(2.0), 4.);
    EXPECT_EQ(lipol.evaluate(2.5), 3.);
    EXPECT_EQ(lipol.evaluate(3.0), 2.);
    EXPECT_EQ(lipol.evaluate(4.0), 2.);
}

TEST(PiecewiseLinearTest, single_interval)
{
    std::vector<Real> x = { 1, 2 };
    std::vector<Real> y = { 0, 4 };
    PiecewiseLinear lipol(x, y);

    EXPECT_EQ(lipol.evaluate(0.5), 0.);
    EXPECT_EQ(lipol.evaluate(1.0), 0.);
    EXPECT_EQ(lipol.evaluate(1.5), 2.);
    EXPECT_EQ(lipol.evaluate(2.0), 4.);
    EXPECT_EQ(lipol.evaluate(2.5), 4.);
}

TEST(PiecewiseLinearTest, single_point)
{
    EXPECT_DEATH(PiecewiseLinear ipol({ 1 }, { 0 }), "Size of 'x' is 1. It must be 2 or more");
}

TEST(PiecewiseLinearTest, unequal_sizes)
{
    EXPECT_DEATH(PiecewiseLinear ipol({ 1, 2 }, { 0, 2, 3 }),
                 "Size of 'x' \\(2\\) does not match size of 'y' \\(3\\)");
}

TEST(PiecewiseLinearTest, non_increasing)
{
    EXPECT_THAT(
        []() { PiecewiseLinear ipol({ 1, 2, 1 }, { 0, 2, 3 }); },
        testing::ThrowsMessage<Exception>("Values in 'x' must be increasing. Failed at index '2'"));
}

TEST(PiecewiseLinearTest, check_x_for_nans)
{
    std::vector<double> x = { 1.0, 2.5, std::numeric_limits<double>::quiet_NaN(), 4.2 };
    EXPECT_DEATH(PiecewiseLinear ipol(x, { 0., 2., 3., 4. }), "Parameter 'x' contains a NaN");
}

TEST(PiecewiseLinearTest, check_y_for_nans)
{
    std::vector<double> y = { 1.0, 2.5, std::numeric_limits<double>::quiet_NaN(), 4.2 };
    EXPECT_DEATH(PiecewiseLinear ipol({ 0., 2., 3., 4. }, y), "Parameter 'y' contains a NaN");
}

TEST(PiecewiseLinearTest, eval_uninitialized_object)
{
    PiecewiseLinear fn;
    EXPECT_DEATH(fn.evaluate(0.), "PiecewiseLinear is not initialized");
}

TEST(PiecewiseLinearTest, pass_into_parameters)
{
    Parameters params;
    params.add_param<PiecewiseLinear<Real>>("fn", "");

    PiecewiseLinear fn({ 1, 2 }, { -1, 3 });
    params.set<PiecewiseLinear<Real>>("fn", fn);

    auto par_fn = params.get<PiecewiseLinear<Real>>("fn");
    EXPECT_NEAR(par_fn.evaluate(1.5), 1., 1e-15);
}
