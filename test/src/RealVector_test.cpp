#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "RealVector.h"

using namespace godzilla;

TEST(RealVectorTest, empty_ctor_1d)
{
    RealVector1D v;
    EXPECT_EQ(v[0], 0.);
}

TEST(RealVectorTest, ctor_1d)
{
    RealVector1D v(1.);
    EXPECT_EQ(v[0], 1.);

    const RealVector1D & w = v;
    EXPECT_EQ(w[0], 1.);
}

TEST(RealVectorTest, dot_product_1d)
{
    RealVector1D v1(2.);
    RealVector1D v2(3.);
    EXPECT_EQ(v1 * v2, 6.);
}

//

TEST(RealVectorTest, empty_ctor_2d)
{
    RealVector2D v;
    EXPECT_EQ(v[0], 0.);
    EXPECT_EQ(v[1], 0.);
}

TEST(RealVectorTest, ctor_2d)
{
    RealVector2D v(1., 2.);
    EXPECT_EQ(v[0], 1.);
    EXPECT_EQ(v[1], 2.);

    const RealVector2D & w = v;
    EXPECT_EQ(w[0], 1.);
    EXPECT_EQ(w[1], 2.);
}

TEST(RealVectorTest, dot_product_2d)
{
    RealVector2D v1(2., 3.);
    RealVector2D v2(4., 5.);
    EXPECT_EQ(v1 * v2, 23.);
}

//

TEST(RealVectorTest, empty_ctor_3d)
{
    RealVector3D v;
    EXPECT_EQ(v[0], 0.);
    EXPECT_EQ(v[1], 0.);
    EXPECT_EQ(v[2], 0.);
}

TEST(RealVectorTest, ctor_3d)
{
    RealVector3D v(1., 2., 3);
    EXPECT_EQ(v[0], 1.);
    EXPECT_EQ(v[1], 2.);
    EXPECT_EQ(v[2], 3.);

    const RealVector3D & w = v;
    EXPECT_EQ(w[0], 1.);
    EXPECT_EQ(w[1], 2.);
    EXPECT_EQ(w[2], 3.);
}

TEST(RealVectorTest, dot_product_3d)
{
    RealVector3D v1(2., 3., 4);
    RealVector3D v2(5., 6., 7.);
    EXPECT_EQ(v1 * v2, 56.);
}
