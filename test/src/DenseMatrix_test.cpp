#include "Common.h"
#include "DenseMatrix.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(DenseMatrixTest, ctor_square)
{
    DenseMatrix<Real> m(2);
    m.zero();
    EXPECT_EQ(m[0][0], 0.);
    EXPECT_EQ(m[0][1], 0.);
    EXPECT_EQ(m[1][0], 0.);
    EXPECT_EQ(m[1][1], 0.);
}

TEST(DenseMatrixTest, ctor_rect)
{
    DenseMatrix<Real> m(2, 3);
    m.zero();
    EXPECT_EQ(m[0][0], 0.);
    EXPECT_EQ(m[0][1], 0.);
    EXPECT_EQ(m[0][2], 0.);
    EXPECT_EQ(m[1][0], 0.);
    EXPECT_EQ(m[1][1], 0.);
    EXPECT_EQ(m[1][2], 0.);
}

TEST(DenseMatrixTest, transpose_sq)
{
    DenseMatrix<Real> m(2, 2);
    m[0][0] = 1;
    m[0][1] = 3;
    m[1][0] = 4;
    m[1][1] = 2;
    m.transpose();

    EXPECT_EQ(m[0][0], 1.);
    EXPECT_EQ(m[0][1], 4.);
    EXPECT_EQ(m[1][0], 3.);
    EXPECT_EQ(m[1][1], 2.);
}

TEST(DenseMatrixTest, chsgn)
{
    DenseMatrix<Real> m(2, 2);
    m[0][0] = 1;
    m[0][1] = 2;
    m[1][0] = 3;
    m[1][1] = 4;
    m.chsgn();

    EXPECT_EQ(m[0][0], -1.);
    EXPECT_EQ(m[0][1], -2.);
    EXPECT_EQ(m[1][0], -3.);
    EXPECT_EQ(m[1][1], -4.);
}
