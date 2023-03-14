#include "gmock/gmock.h"
#include "DenseMatrixSymm.h"
#include "DenseVector.h"
#include "DenseMatrix.h"
#include "Types.h"

using namespace godzilla;
using namespace testing;

TEST(DenseMatrixSymmTest, set)
{
    DenseMatrixSymm<Real, 3> m;
    m.set(0, 0) = 1.;
    m.set(0, 1) = 0.;
    m.set(0, 2) = 3.;
    m.set(1, 1) = -1.;
    m.set(1, 2) = 4.;
    m.set(2, 2) = 2.;

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 3.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), -1.);
    EXPECT_EQ(m(1, 2), 4.);
    EXPECT_EQ(m(2, 0), 3.);
    EXPECT_EQ(m(2, 1), 4.);
    EXPECT_EQ(m(2, 2), 2.);
}

TEST(DenseMatrixSymmTest, set2)
{
    DenseMatrixSymm<Real, 3> m;
    m.set(0, 0, 1.);
    m.set(0, 1, 0.);
    m.set(0, 2, 3.);
    m.set(1, 1, -1.);
    m.set(1, 2, 4.);
    m.set(2, 2, 2.);

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 3.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), -1.);
    EXPECT_EQ(m(1, 2), 4.);
    EXPECT_EQ(m(2, 0), 3.);
    EXPECT_EQ(m(2, 1), 4.);
    EXPECT_EQ(m(2, 2), 2.);
}

TEST(DenseMatrixSymmTest, set_values_1)
{
    DenseMatrixSymm<Real, 3> m;
    m.set_values(2.);
    for (Int i = 0; i < 3; i++)
        for (Int j = 0; j < 3; j++)
            EXPECT_EQ(m(i, j), 2.);
}

TEST(DenseMatrixSymmTest, set_values_vals)
{
    DenseMatrixSymm<Real, 3> m;
    m.set_values({ 2., 1., 2., 0., -1., -2. });
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), -1.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), -1.);
    EXPECT_EQ(m(2, 2), -2.);
}

TEST(DenseMatrixSymmTest, scale)
{
    DenseMatrixSymm<Real, 3> m({ 2., 1., 2., 0., -1., -2. });
    m.scale(3.);
    EXPECT_EQ(m(0, 0), 6.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), 6.);
    EXPECT_EQ(m(1, 2), -3.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), -3.);
    EXPECT_EQ(m(2, 2), -6.);
}

TEST(DenseMatrixSymmTest, mult)
{
    DenseMatrixSymm<Real, 3> m({ 1, 1, 1, 0, 1, 1 });
    DenseVector<Real, 3> v({ 2., 3., 4. });

    DenseVector<Real, 3> res = m.mult(v);
    EXPECT_EQ(res(0), 5.);
    EXPECT_EQ(res(1), 9.);
    EXPECT_EQ(res(2), 7.);
}

TEST(DenseMatrixSymmTest, op_mult)
{
    DenseMatrixSymm<Real, 3> m({ 1, 1, 1, 0, 1, 1 });
    DenseVector<Real, 3> v({ 2., 3., 4. });

    DenseVector<Real, 3> res = m * v;
    EXPECT_EQ(res(0), 5.);
    EXPECT_EQ(res(1), 9.);
    EXPECT_EQ(res(2), 7.);
}

TEST(DenseMatrixSymmTest, det1)
{
    auto m = DenseMatrixSymm<Real, 1>();
    m(0, 0) = 2.;
    EXPECT_EQ(m.det(), 2.);
}

TEST(DenseMatrixSymmTest, det2)
{
    auto m = DenseMatrixSymm<Real, 2>();
    m.set_values({ 2, 3, 4 });
    EXPECT_EQ(m.det(), -1.);
}

TEST(DenseMatrixSymmTest, det3)
{
    auto m = DenseMatrixSymm<Real, 3>();
    m.set_values({ 2, -3, 6, 4, 7, -1 });
    EXPECT_EQ(m.det(), -365.);
}

TEST(DenseMatrixSymmTest, det4)
{
    auto m = DenseMatrixSymm<Real, 4>();
    m.set_values({ 1, 0, 2, 2, -1, 0, -1, 3, 2, 2 });
    //    m.set_row(0, { 1, 0, 2, -1 });
    //    m.set_row(1, { 1, 2, -1, 3 });
    //    m.set_row(2, { 0, 1, 0, 2 });
    //    m.set_row(3, { 1, -2, -3, 2 });
    //    EXPECT_EQ(m.det(), 21.);
    EXPECT_DEATH(m.det(), "Determinant is not implemented for 4x4 matrices, yet.");
}

TEST(DenseMatrixSymmTest, transpose3)
{
    auto m = DenseMatrixSymm<Real, 3>();
    m.set_values({ 2, -3, 6, 4, -7, -2 });
    auto tr = m.trans();
    EXPECT_EQ(tr(0, 0), 2.);
    EXPECT_EQ(tr(0, 1), -3.);
    EXPECT_EQ(tr(0, 2), 4.);
    EXPECT_EQ(tr(1, 0), -3.);
    EXPECT_EQ(tr(1, 1), 6.);
    EXPECT_EQ(tr(1, 2), -7.);
    EXPECT_EQ(tr(2, 0), 4.);
    EXPECT_EQ(tr(2, 1), -7.);
    EXPECT_EQ(tr(2, 2), -2.);
}

TEST(DenseMatrixSymmTest, mult_mat)
{
    auto a = DenseMatrixSymm<Real, 3>();
    a.set_values({ 2, -3, 6, 4, -7, -1 });
    auto b = DenseMatrixSymm<Real, 3>();
    b.set_values({ -1, 0, 1, 2, -2, 1 });
    auto m = a * b;
    EXPECT_EQ(m(0, 0), 6.);
    EXPECT_EQ(m(0, 1), -11.);
    EXPECT_EQ(m(0, 2), 14.);
    EXPECT_EQ(m(1, 0), -11.);
    EXPECT_EQ(m(1, 1), 20.);
    EXPECT_EQ(m(1, 2), -25.);
    EXPECT_EQ(m(2, 0), -6.);
    EXPECT_EQ(m(2, 1), -5.);
    EXPECT_EQ(m(2, 2), 21.);
}
