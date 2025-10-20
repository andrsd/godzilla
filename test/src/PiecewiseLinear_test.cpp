#include "gmock/gmock.h"
#include "godzilla/PiecewiseLinear.h"
#include "godzilla/Exception.h"

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

TEST(PiecewiseLinearTest, ctor_empty)
{
    PiecewiseLinear lipol;
    std::vector<Real> x = { 1, 2, 3, 4 };
    std::vector<Real> y = { 0, 4, 2, 3 };
    lipol.create(x, y);

    EXPECT_EQ(lipol.evaluate(0.0), 0.);
    EXPECT_EQ(lipol.evaluate(1.0), 0.);
    EXPECT_EQ(lipol.evaluate(1.5), 2.);
    EXPECT_EQ(lipol.evaluate(2.0), 4.);
    EXPECT_EQ(lipol.evaluate(2.5), 3.);
    EXPECT_EQ(lipol.evaluate(3.0), 2.);
    EXPECT_EQ(lipol.evaluate(3.5), 2.5);
    EXPECT_EQ(lipol.evaluate(4.0), 3.);
    EXPECT_EQ(lipol.evaluate(4.5), 3.);
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
