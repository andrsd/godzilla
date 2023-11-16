#include "gmock/gmock.h"
#include "godzilla/Random.h"

using namespace godzilla;

TEST(RandomTest, scalar)
{
    Random rng;
    rng.create(PETSC_COMM_SELF);

    rng.set_type(PETSCRAND48);
    EXPECT_STREQ(rng.get_type().c_str(), PETSCRAND48);

    rng.set_seed(0);
    rng.seed();
    EXPECT_EQ(rng.get_seed(), 0);

    rng.set_interval(0, 100);
    Scalar l, h;
    rng.get_interval(l, h);
    EXPECT_EQ(l, 0);
    EXPECT_EQ(h, 100);

    EXPECT_DOUBLE_EQ(rng.get_value(), 17.082803610628972);

    std::vector<Scalar> rv(3);
    rng.get_values(rv);
    EXPECT_DOUBLE_EQ(rv[0], 74.990198048496381);
    EXPECT_DOUBLE_EQ(rv[1], 9.637165562356742);
    EXPECT_DOUBLE_EQ(rv[2], 87.046522702707563);

    rng.destroy();
}

TEST(RandomTest, real)
{
    Random rng;
    rng.create(PETSC_COMM_SELF);

    rng.set_type(PETSCRAND48);
    EXPECT_STREQ(rng.get_type().c_str(), PETSCRAND48);

    rng.set_seed(0);
    rng.seed();
    EXPECT_EQ(rng.get_seed(), 0);

    rng.set_interval(0, 100);
    Scalar l, h;
    rng.get_interval(l, h);
    EXPECT_EQ(l, 0);
    EXPECT_EQ(h, 100);

    EXPECT_DOUBLE_EQ(rng.get_value_real(), 17.082803610628972);

    std::vector<Real> rv(3);
    rng.get_values_real(rv);
    EXPECT_DOUBLE_EQ(rv[0], 74.990198048496381);
    EXPECT_DOUBLE_EQ(rv[1], 9.637165562356742);
    EXPECT_DOUBLE_EQ(rv[2], 87.046522702707563);

    rng.destroy();
}
