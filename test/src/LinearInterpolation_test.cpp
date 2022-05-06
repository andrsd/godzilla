#include "gtest/gtest.h"
#include "Godzilla.h"
#include "LinearInterpolation.h"

using namespace godzilla;

TEST(LinearInterpolationTest, sample)
{
    std::vector<PetscReal> x = { 1, 2, 3 };
    std::vector<PetscReal> y = { 0, 4, 2 };
    LinearInterpolation lipol(x, y);

    EXPECT_EQ(lipol.sample(0.0), 0.);
    EXPECT_EQ(lipol.sample(1.0), 0.);
    EXPECT_EQ(lipol.sample(1.5), 2.);
    EXPECT_EQ(lipol.sample(2.0), 4.);
    EXPECT_EQ(lipol.sample(2.5), 3.);
    EXPECT_EQ(lipol.sample(3.0), 2.);
    EXPECT_EQ(lipol.sample(4.0), 2.);
}

TEST(LinearInterpolationTest, ctor_empty)
{
    LinearInterpolation lipol;
    std::vector<PetscReal> x = { 1, 2, 3, 4 };
    std::vector<PetscReal> y = { 0, 4, 2, 3 };
    lipol.create(x, y);

    EXPECT_EQ(lipol.sample(0.0), 0.);
    EXPECT_EQ(lipol.sample(1.0), 0.);
    EXPECT_EQ(lipol.sample(1.5), 2.);
    EXPECT_EQ(lipol.sample(2.0), 4.);
    EXPECT_EQ(lipol.sample(2.5), 3.);
    EXPECT_EQ(lipol.sample(3.0), 2.);
    EXPECT_EQ(lipol.sample(3.5), 2.5);
    EXPECT_EQ(lipol.sample(4.0), 3.);
    EXPECT_EQ(lipol.sample(4.5), 3.);
}

TEST(LinearInterpolationTest, single_interval)
{
    std::vector<PetscReal> x = { 1, 2 };
    std::vector<PetscReal> y = { 0, 4 };
    LinearInterpolation lipol(x, y);

    EXPECT_EQ(lipol.sample(0.5), 0.);
    EXPECT_EQ(lipol.sample(1.0), 0.);
    EXPECT_EQ(lipol.sample(1.5), 2.);
    EXPECT_EQ(lipol.sample(2.0), 4.);
    EXPECT_EQ(lipol.sample(2.5), 4.);
}

TEST(LinearInterpolationTest, single_point)
{
    std::vector<PetscReal> x = { 1 };
    std::vector<PetscReal> y = { 0 };
    EXPECT_DEATH(LinearInterpolation(x, y),
                 "error: LinearInterpolation: Size of 'x' is 1. It must be 2 or more.");
}

TEST(LinearInterpolationTest, unequal_sizes)
{
    std::vector<PetscReal> x = { 1, 2 };
    std::vector<PetscReal> y = { 0, 2, 3 };
    EXPECT_DEATH(
        LinearInterpolation(x, y),
        "error: LinearInterpolation: size of 'x' \\(2\\) does not match size of 'y' \\(3\\)");
}

TEST(LinearInterpolationTest, non_increasing)
{
    std::vector<PetscReal> x = { 1, 2, 1 };
    std::vector<PetscReal> y = { 0, 2, 3 };
    EXPECT_DEATH(
        LinearInterpolation(x, y),
        "error: LinearInterpolation: Values in 'x' must be increasing. Failed at index '2'.");
}
