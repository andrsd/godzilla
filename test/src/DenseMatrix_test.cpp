#include "gmock/gmock.h"
#include "DenseMatrix.h"
#include "DenseMatrixSymm.h"
#include "DenseVector.h"
#include "Types.h"

using namespace godzilla;
using namespace testing;

TEST(DenseMatrixTest, set)
{
    DenseMatrix<Real, 2> m;
    m.set(0, 0) = 1.;
    m.set(0, 1) = 0.;
    m.set(1, 0) = 2.;
    m.set(1, 1) = -1.;

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -1.);
}

TEST(DenseMatrixTest, set2)
{
    DenseMatrix<Real, 2> m;
    m.set(0, 0, 1.);
    m.set(0, 1, 0.);
    m.set(1, 0, 2.);
    m.set(1, 1, -1);

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -1.);
}

TEST(DenseMatrixTest, set_values)
{
    DenseMatrix<Real, 2> m;
    m.set_values(3.);
    EXPECT_EQ(m(0, 0), 3.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), 3.);
}

TEST(DenseMatrixTest, scale)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 2, 1, 0 });
    m.set_row(1, { 1, 2, -1 });
    m.set_row(2, { 0, -1, 2 });
    m.scale(3.);
    EXPECT_EQ(m(0, 0), 6.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), 6.);
    EXPECT_EQ(m(1, 2), -3.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), -3.);
    EXPECT_EQ(m(2, 2), 6.);
}

TEST(DenseMatrixTest, mult)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 1, 1, 0 });
    m.set_row(1, { 1, 1, 1 });
    m.set_row(2, { 0, 1, 1 });
    DenseVector<Real, 3> v({ 2., 3., 4. });

    DenseVector<Real, 3> res = m.mult(v);
    EXPECT_EQ(res(0), 5.);
    EXPECT_EQ(res(1), 9.);
    EXPECT_EQ(res(2), 7.);
}

TEST(DenseMatrixTest, op_mult_scalar)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 2, 1, 0 });
    m.set_row(1, { -1, -2, 4 });
    m.set_row(2, { 0, 3, -1 });

    auto res = m * 2.;
    EXPECT_EQ(res(0, 0), 4.);
    EXPECT_EQ(res(0, 1), 2.);
    EXPECT_EQ(res(0, 2), 0.);
    EXPECT_EQ(res(1, 0), -2.);
    EXPECT_EQ(res(1, 1), -4.);
    EXPECT_EQ(res(1, 2), 8.);
    EXPECT_EQ(res(2, 0), 0.);
    EXPECT_EQ(res(2, 1), 6.);
    EXPECT_EQ(res(2, 2), -2.);
}

TEST(DenseMatrixTest, op_mult)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 1, 1, 0 });
    m.set_row(1, { 1, 1, 1 });
    m.set_row(2, { 0, 1, 1 });
    DenseVector<Real, 3> v({ 2., 3., 4. });

    DenseVector<Real, 3> res = m * v;
    EXPECT_EQ(res(0), 5.);
    EXPECT_EQ(res(1), 9.);
    EXPECT_EQ(res(2), 7.);
}

TEST(DenseMatrixTest, create_symm_1)
{
    auto m = DenseMatrix<Real, 1>::create_symm({ 2 });
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m.get_num_cols(), 1);
    EXPECT_EQ(m.get_num_rows(), 1);
}

TEST(DenseMatrixTest, create_symm_2)
{
    auto m = DenseMatrix<Real, 2>::create_symm({ 2, 1, -2 });
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), -2.);
    EXPECT_EQ(m.get_num_cols(), 2);
    EXPECT_EQ(m.get_num_rows(), 2);
}

TEST(DenseMatrixTest, create_symm_3)
{
    auto m = DenseMatrix<Real, 3>::create_symm({ 2, 1, 0, -2, 1, 3 });
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 1);
    EXPECT_EQ(m(1, 1), -2.);
    EXPECT_EQ(m(1, 2), 1.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), 1.);
    EXPECT_EQ(m(2, 2), 3.);
    EXPECT_EQ(m.get_num_cols(), 3);
    EXPECT_EQ(m.get_num_rows(), 3);
}

void
fail_to_create_symm()
{
    auto m = DenseMatrix<Real, 3>::create_symm({ 2 });
}

TEST(DenseMatrixDeathTest, create_symm_not_enough_vals)
{
    EXPECT_DEATH(fail_to_create_symm(), "Assertion.+failed");
}

TEST(DenseMatrixTest, det1)
{
    auto m = DenseMatrix<Real, 1>();
    m(0, 0) = 2.;
    EXPECT_EQ(m.det(), 2.);
}

TEST(DenseMatrixTest, det2)
{
    auto m = DenseMatrix<Real, 2>();
    m.set_row(0, { 2, 3 });
    m.set_row(1, { 4, 5 });
    EXPECT_EQ(m.det(), -2.);
}

TEST(DenseMatrixTest, det3)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, 7 });
    m.set_row(2, { 8, 9, -1 });
    EXPECT_EQ(m.det(), -663.);
}

TEST(DenseMatrixTest, det4)
{
    auto m = DenseMatrix<Real, 4>();
    m.set_row(0, { 1, 0, 2, -1 });
    m.set_row(1, { 1, 2, -1, 3 });
    m.set_row(2, { 0, 1, 0, 2 });
    m.set_row(3, { 1, -2, -3, 2 });
    //    EXPECT_EQ(m.det(), 21.);
    EXPECT_DEATH(m.det(), "Determinant is not implemented for 4x4 matrices, yet.");
}

TEST(DenseMatrixTest, inv1)
{
    auto m = DenseMatrix<Real, 1>();
    m(0, 0) = 2.;
    auto inv = m.inv();
    EXPECT_EQ(inv(0, 0), 1. / 2.);
}

TEST(DenseMatrixTest, inv2)
{
    auto m = DenseMatrix<Real, 2>();
    m.set_row(0, { 2, 3 });
    m.set_row(1, { 4, 5 });
    auto inv = m.inv();
    EXPECT_EQ(inv(0, 0), -5. / 2);
    EXPECT_EQ(inv(0, 1), 3. / 2.);
    EXPECT_EQ(inv(1, 0), 2.);
    EXPECT_EQ(inv(1, 1), -1.);
}

TEST(DenseMatrixDeathTest, inv2_singular)
{
    auto m = DenseMatrix<Real, 2>();
    m.set_row(0, { 1, 2 });
    m.set_row(1, { 2, 4 });
    EXPECT_DEATH(m.inv(), "Inverting of a matrix failed: matrix is singular.");
}

TEST(DenseMatrixTest, inv3)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, -7 });
    m.set_row(2, { 8, -1, -2 });
    auto inv = m.inv();
    EXPECT_DOUBLE_EQ(inv(0, 0), 19. / 12.);
    EXPECT_DOUBLE_EQ(inv(0, 1), 5. / 6.);
    EXPECT_DOUBLE_EQ(inv(0, 2), 1. / 4.);
    EXPECT_DOUBLE_EQ(inv(1, 0), 11. / 2.);
    EXPECT_DOUBLE_EQ(inv(1, 1), 3.);
    EXPECT_DOUBLE_EQ(inv(1, 2), 1. / 2.);
    EXPECT_DOUBLE_EQ(inv(2, 0), 43. / 12.);
    EXPECT_DOUBLE_EQ(inv(2, 1), 11. / 6.);
    EXPECT_DOUBLE_EQ(inv(2, 2), 1. / 4.);
}

TEST(DenseMatrixDeathTest, inv3_singular)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 1, 2, 3 });
    m.set_row(1, { 2, 4, 6 });
    m.set_row(2, { 1, 1, 1 });
    EXPECT_DEATH(m.inv(), "Inverting of a matrix failed: matrix is singular.");
}

TEST(DenseMatrixDeathTest, inv4)
{
    auto m = DenseMatrix<Real, 4>();
    EXPECT_DEATH(m.inv(), "Inverse is not implemented for 4x4 matrices, yet.");
}

TEST(DenseMatrixTest, transpose3)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, -7 });
    m.set_row(2, { 8, -1, -2 });
    auto tr = m.trans();
    EXPECT_EQ(tr(0, 0), 2.);
    EXPECT_EQ(tr(0, 1), -5.);
    EXPECT_EQ(tr(0, 2), 8.);
    EXPECT_EQ(tr(1, 0), -3.);
    EXPECT_EQ(tr(1, 1), 6.);
    EXPECT_EQ(tr(1, 2), -1.);
    EXPECT_EQ(tr(2, 0), 4.);
    EXPECT_EQ(tr(2, 1), -7.);
    EXPECT_EQ(tr(2, 2), -2.);
}

TEST(DenseMatrixTest, mult_mat)
{
    auto a = DenseMatrix<Real, 2, 3>();
    a.set_row(0, { 2, -3, 4 });
    a.set_row(1, { -5, 6, -7 });
    auto b = DenseMatrix<Real, 3, 4>();
    b.set_row(0, { -1, 0, 2, 3 });
    b.set_row(1, { -5, 1, -2, 1 });
    b.set_row(2, { -4, 2, 0, 1 });
    auto m = a * b;
    EXPECT_EQ(m(0, 0), -3.);
    EXPECT_EQ(m(0, 1), 5.);
    EXPECT_EQ(m(0, 2), 10.);
    EXPECT_EQ(m(0, 3), 7.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), -8.);
    EXPECT_EQ(m(1, 2), -22.);
    EXPECT_EQ(m(1, 3), -16.);
}

TEST(DenseMatrixTest, copy_ctor)
{
    DenseMatrixSymm<Real, 3> symm({ 1, -1, 2, 3, -2, -1 });
    DenseMatrix<Real, 3> a(symm);
    EXPECT_EQ(a(0, 0), 1);
    EXPECT_EQ(a(0, 1), -1);
    EXPECT_EQ(a(0, 2), 3);
    EXPECT_EQ(a(1, 0), -1);
    EXPECT_EQ(a(1, 1), 2);
    EXPECT_EQ(a(1, 2), -2);
    EXPECT_EQ(a(2, 0), 3);
    EXPECT_EQ(a(2, 1), -2);
    EXPECT_EQ(a(2, 2), -1);
}

TEST(DenseMatrixTest, op_assign)
{
    DenseMatrixSymm<Real, 3> symm({ 1, -1, 2, 3, -2, -1 });
    DenseMatrix<Real, 3> a;
    a = symm;
    EXPECT_EQ(a(0, 0), 1);
    EXPECT_EQ(a(0, 1), -1);
    EXPECT_EQ(a(0, 2), 3);
    EXPECT_EQ(a(1, 0), -1);
    EXPECT_EQ(a(1, 1), 2);
    EXPECT_EQ(a(1, 2), -2);
    EXPECT_EQ(a(2, 0), 3);
    EXPECT_EQ(a(2, 1), -2);
    EXPECT_EQ(a(2, 2), -1);
}
