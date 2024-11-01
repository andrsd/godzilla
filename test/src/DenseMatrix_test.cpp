#include "gmock/gmock.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/DenseMatrixSymm.h"
#include "godzilla/DenseVector.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;
using namespace testing;

TEST(DenseMatrixTest, zero)
{
    DenseMatrix<Real, 2> m;
    m.set(0, 0) = 1.;
    m.set(0, 1) = -2.;
    m.set(1, 0) = 2.;
    m.set(1, 1) = -1.;
    m.zero();

    EXPECT_EQ(m(0, 0), 0.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 0.);
}

TEST(DenseMatrixTest, zero_non_fundamental_type)
{
    DenseMatrix<DenseVector<Real, 2>, 1, 2> m;
    m.zero();

    EXPECT_EQ(m(0, 0)(0), 0.);
    EXPECT_EQ(m(0, 0)(1), 0.);
    EXPECT_EQ(m(0, 1)(0), 0.);
    EXPECT_EQ(m(0, 1)(1), 0.);
}

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

TEST(DenseMatrixTest, op_unary_minus)
{
    DenseMatrix<Real, 2> m;
    m.set(0, 0) = 1.;
    m.set(0, 1) = -2.;
    m.set(1, 0) = 2.;
    m.set(1, 1) = 0.;
    auto n = -m;
    EXPECT_EQ(n(0, 0), -1.);
    EXPECT_EQ(n(0, 1), 2.);
    EXPECT_EQ(n(1, 0), -2.);
    EXPECT_EQ(n(1, 1), 0.);
}

TEST(DenseMatrixTest, add)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrix<Real, 2> n;
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    m.add(n);
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), -3.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -3.);
}

TEST(DenseMatrixTest, add_symm)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrixSymm<Real, 2> n;
    n.set_values({ 1., -1., -3. });
    m.add(n);
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), -3.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), -3.);
}

TEST(DenseMatrixTest, op_add)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrix<Real, 2> n;
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    auto o = m + n;
    EXPECT_EQ(o(0, 0), 2.);
    EXPECT_EQ(o(0, 1), -3.);
    EXPECT_EQ(o(1, 0), 2.);
    EXPECT_EQ(o(1, 1), -3.);
}

TEST(DenseMatrixTest, op_add_symm)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrixSymm<Real, 2> n;
    n.set_values({ 1., -1., -3. });
    auto o = m + n;
    EXPECT_EQ(o(0, 0), 2.);
    EXPECT_EQ(o(0, 1), -3.);
    EXPECT_EQ(o(1, 0), 1.);
    EXPECT_EQ(o(1, 1), -3.);
}

TEST(DenseMatrixTest, op_inc_mat)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 1, 1, 0 });
    m.set_row(1, { 1, 1, 1 });
    m.set_row(2, { 0, 1, 1 });

    DenseMatrix<Real, 3> n;
    n.set_row(0, { 2, -1, 1 });
    n.set_row(1, { 1, -3, 4 });
    n.set_row(2, { -1, -1, -2 });

    m += n;
    EXPECT_EQ(m(0, 0), 3.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -2.);
    EXPECT_EQ(m(1, 2), 5.);
    EXPECT_EQ(m(2, 0), -1.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), -1.);
}

TEST(DenseMatrixTest, subtract)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrix<Real, 2> n;
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    m.subtract(n);
    EXPECT_EQ(m(0, 0), 0.);
    EXPECT_EQ(m(0, 1), -1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), 3.);
}

TEST(DenseMatrixTest, subtract_symm)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrixSymm<Real, 2> n;
    n.set_values({ 1., -1., -3. });
    m.subtract(n);
    EXPECT_EQ(m(0, 0), 0.);
    EXPECT_EQ(m(0, 1), -1.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), 3.);
}

TEST(DenseMatrixTest, op_subtract)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrix<Real, 2> n;
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    auto o = m - n;
    EXPECT_EQ(o(0, 0), 0.);
    EXPECT_EQ(o(0, 1), -1.);
    EXPECT_EQ(o(1, 0), 2.);
    EXPECT_EQ(o(1, 1), 3.);
}

TEST(DenseMatrixTest, op_subtract_symm)
{
    DenseMatrix<Real, 2> m;
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DenseMatrixSymm<Real, 2> n;
    n.set_values({ -1., 1., -4. });
    auto o = m - n;
    EXPECT_EQ(o(0, 0), 2.);
    EXPECT_EQ(o(0, 1), -3.);
    EXPECT_EQ(o(1, 0), 1.);
    EXPECT_EQ(o(1, 1), 4.);
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

TEST(DenseMatrixTest, op_mult_symm)
{
    DenseMatrix<Real, 2, 3> m;
    m.set_row(0, { -1., 2., 1. });
    m.set_row(1, { 0., 1., -2. });

    DenseMatrixSymm<Real, 3> ms;
    ms(0, 0) = 1;
    ms(0, 1) = 2;
    ms(0, 2) = 0;
    ms(1, 1) = 3;
    ms(1, 2) = 4;
    ms(2, 2) = 5;

    auto res = m * ms;
    EXPECT_EQ(res(0, 0), 3);
    EXPECT_EQ(res(0, 1), 8);
    EXPECT_EQ(res(0, 2), 13);
    EXPECT_EQ(res(1, 0), 2);
    EXPECT_EQ(res(1, 1), -5);
    EXPECT_EQ(res(1, 2), -6);
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
    [[maybe_unused]] auto m = DenseMatrix<Real, 3>::create_symm({ 2 });
}

#ifndef NDEBUG

TEST(DenseMatrixDeathTest, create_symm_not_enough_vals)
{
    EXPECT_DEATH(fail_to_create_symm(), "Assertion.+failed");
}

#endif

TEST(DenseMatrixTest, create_diagonal)
{
    auto v = DenseVector<Real, 3>({ 1, 2, 3 });
    auto m = DenseMatrix<Real, 3>::create_diagonal(v);
    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), 0.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), 3.);
}

TEST(DenseMatrixTest, det1)
{
    auto m = DenseMatrix<Real, 1>();
    m(0, 0) = 2.;
    EXPECT_EQ(determinant(m), 2.);
}

TEST(DenseMatrixTest, det2)
{
    auto m = DenseMatrix<Real, 2>();
    m.set_row(0, { 2, 3 });
    m.set_row(1, { 4, 5 });
    EXPECT_EQ(determinant(m), -2.);
}

TEST(DenseMatrixTest, det3)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, 7 });
    m.set_row(2, { 8, 9, -1 });
    EXPECT_EQ(determinant(m), -663.);
}

TEST(DenseMatrixTest, det4)
{
    auto m = DenseMatrix<Real, 4>();
    m.set_row(0, { 1, 0, 2, -1 });
    m.set_row(1, { 1, 2, -1, 3 });
    m.set_row(2, { 0, 1, 0, 2 });
    m.set_row(3, { 1, -2, -3, 2 });
    //    EXPECT_EQ(m.det(), 21.);
    EXPECT_THROW_MSG({ [[maybe_unused]] auto d = determinant(m); },
                     "Determinant is not implemented for 4x4 matrices, yet");
}

TEST(DenseMatrixTest, inv1)
{
    auto m = DenseMatrix<Real, 1>();
    m(0, 0) = 2.;
    auto inv = inverse(m);
    EXPECT_EQ(inv(0, 0), 1. / 2.);
}

TEST(DenseMatrixTest, inv2)
{
    auto m = DenseMatrix<Real, 2>();
    m.set_row(0, { 2, 3 });
    m.set_row(1, { 4, 5 });
    auto inv = inverse(m);
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
    EXPECT_THROW_MSG(inverse(m), "Inverting of a matrix failed: matrix is singular.");
}

TEST(DenseMatrixTest, inv3)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, -7 });
    m.set_row(2, { 8, -1, -2 });
    auto inv = inverse(m);
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
    EXPECT_THROW_MSG(inverse(m), "Inverting of a matrix failed: matrix is singular.");
}

TEST(DenseMatrixDeathTest, inv4)
{
    auto m = DenseMatrix<Real, 4>();
    EXPECT_THROW_MSG(inverse(m), "Inverse is not implemented for 4x4 matrices, yet");
}

TEST(DenseMatrixTest, transpose3)
{
    auto m = DenseMatrix<Real, 3>();
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, -7 });
    m.set_row(2, { 8, -1, -2 });
    auto tr = m.transpose();
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

TEST(DenseMatrixTest, op_mult_scalar_pre)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 2, 1, 0 });
    m.set_row(1, { 1, 2, -1 });
    m.set_row(2, { 0, -1, 2 });
    auto res = 3. * m;
    EXPECT_EQ(res(0, 0), 6.);
    EXPECT_EQ(res(0, 1), 3.);
    EXPECT_EQ(res(0, 2), 0.);
    EXPECT_EQ(res(1, 0), 3.);
    EXPECT_EQ(res(1, 1), 6.);
    EXPECT_EQ(res(1, 2), -3.);
    EXPECT_EQ(res(2, 0), 0.);
    EXPECT_EQ(res(2, 1), -3.);
    EXPECT_EQ(res(2, 2), 6.);
}

TEST(DenseMatrixTest, column)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 2, 1, 5 });
    m.set_row(1, { 3, -1, -2 });
    m.set_row(2, { 0, -3, 4 });

    DenseVector<Real, 3> c0(m.column(0));
    EXPECT_EQ(c0(0), 2.);
    EXPECT_EQ(c0(1), 3.);
    EXPECT_EQ(c0(2), 0.);

    DenseVector<Real, 3> c1(m.column(1));
    EXPECT_EQ(c1(0), 1.);
    EXPECT_EQ(c1(1), -1.);
    EXPECT_EQ(c1(2), -3.);

    DenseVector<Real, 3> c2(m.column(2));
    EXPECT_EQ(c2(0), 5.);
    EXPECT_EQ(c2(1), -2.);
    EXPECT_EQ(c2(2), 4.);
}

TEST(DenseMatrixTest, row)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 2, 1, 5 });
    m.set_row(1, { 3, -1, -2 });
    m.set_row(2, { 0, -3, 4 });

    auto r0 = m.row(0);
    EXPECT_EQ(r0(0, 0), 2.);
    EXPECT_EQ(r0(0, 1), 1.);
    EXPECT_EQ(r0(0, 2), 5.);

    auto r1 = m.row(1);
    EXPECT_EQ(r1(0, 0), 3.);
    EXPECT_EQ(r1(0, 1), -1.);
    EXPECT_EQ(r1(0, 2), -2.);

    auto r2 = m.row(2);
    EXPECT_EQ(r2(0, 0), 0.);
    EXPECT_EQ(r2(0, 1), -3.);
    EXPECT_EQ(r2(0, 2), 4.);
}

TEST(DenseMatrixTest, diagonal)
{
    DenseMatrix<Real, 3> m;
    m.set_row(0, { 2, 1, 5 });
    m.set_row(1, { 3, -1, -2 });
    m.set_row(2, { 0, -3, 4 });
    auto diag = m.diagonal();
    EXPECT_EQ(diag(0), 2.);
    EXPECT_EQ(diag(1), -1.);
    EXPECT_EQ(diag(2), 4.);
}

TEST(DenseMatrixTest, set_row_std_vec)
{
    DenseMatrix<Real, 3, 2> m;
    m.zero();

    m.set_row(0, { 1, 3 });
    m.set_row(1, { 2, 5 });
    m.set_row(2, { 0, 4 });

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(2, 1), 4.);
}

TEST(DenseMatrixTest, set_row_dv)
{
    DenseMatrix<Real, 3, 2> m;
    m.zero();

    m.set_row(0, DenseVector<Real, 2>({ 1, 3 }));
    m.set_row(1, DenseVector<Real, 2>({ 2, 5 }));
    m.set_row(2, DenseVector<Real, 2>({ 0, 4 }));

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(2, 1), 4.);
}

TEST(DenseMatrixTest, set_col)
{
    DenseMatrix<Real, 3, 2> m;
    m.zero();

    m.set_col(0, DenseVector<Real, 3>({ 1, 2, 0 }));
    m.set_col(1, DenseVector<Real, 3>({ 3, 5, 4 }));

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(2, 1), 4.);
}

TEST(DenseMatrixTest, out)
{
    testing::internal::CaptureStdout();

    DenseMatrix<Real, 2, 3> m;
    m.set_row(0, { 1, 2, 3 });
    m.set_row(1, { 6, 5, 4 });

    std::cout << m;

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("(1, 2, 3)"));
    EXPECT_THAT(out, HasSubstr("(6, 5, 4)"));
}

TEST(DenseMatrixTest, transpose)
{
    DenseMatrix<Real, 2, 3> A;
    A(0, 0) = -2;
    A(0, 1) = 5;
    A(0, 2) = 3;
    A(1, 0) = 1;
    A(1, 1) = -1;
    A(1, 2) = 2;

    auto m = transpose(A);
    EXPECT_EQ(m(0, 0), -2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(1, 0), 5.);
    EXPECT_EQ(m(1, 1), -1.);
    EXPECT_EQ(m(2, 0), 3.);
    EXPECT_EQ(m(2, 1), 2.);
}

// --

TEST(DynDenseMatrixTest, ctor_init)
{
    DynDenseMatrix<Real> m(2, 2, 123.);
    EXPECT_EQ(m(0, 0), 123.);
    EXPECT_EQ(m(0, 1), 123.);
    EXPECT_EQ(m(1, 0), 123.);
    EXPECT_EQ(m(1, 1), 123.);
}

TEST(DynDenseMatrixTest, zero)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set(0, 0) = 1.;
    m.set(0, 1) = -2.;
    m.set(1, 0) = 2.;
    m.set(1, 1) = -1.;
    m.zero();

    EXPECT_EQ(m(0, 0), 0.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 0.);
}

TEST(DynDenseMatrixTest, set)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set(0, 0) = 1.;
    m.set(0, 1) = 0.;
    m.set(1, 0) = 2.;
    m.set(1, 1) = -1.;

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -1.);
}

TEST(DynDenseMatrixTest, set2)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set(0, 0, 1.);
    m.set(0, 1, 0.);
    m.set(1, 0, 2.);
    m.set(1, 1, -1);

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -1.);
}

TEST(DynDenseMatrixTest, set_values)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set_values(3.);
    EXPECT_EQ(m(0, 0), 3.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), 3.);
}

TEST(DynDenseMatrixTest, scale)
{
    DynDenseMatrix<Real> m(3, 3);
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

TEST(DynDenseMatrixTest, mult)
{
    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, { 1, 1, 0 });
    m.set_row(1, { 1, 1, 1 });
    m.set_row(2, { 0, 1, 1 });
    DynDenseVector<Real> v(3);
    v.set_values({ 2., 3., 4. });

    auto res = m.mult(v);
    EXPECT_EQ(res(0), 5.);
    EXPECT_EQ(res(1), 9.);
    EXPECT_EQ(res(2), 7.);
}

TEST(DynDenseMatrixTest, op_unary_minus)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set(0, 0) = 1.;
    m.set(0, 1) = -2.;
    m.set(1, 0) = 2.;
    m.set(1, 1) = 0.;
    auto n = -m;
    EXPECT_EQ(n(0, 0), -1.);
    EXPECT_EQ(n(0, 1), 2.);
    EXPECT_EQ(n(1, 0), -2.);
    EXPECT_EQ(n(1, 1), 0.);
}

TEST(DynDenseMatrixTest, add)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DynDenseMatrix<Real> n(2, 2);
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    m.add(n);
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), -3.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -3.);
}

TEST(DynDenseMatrixTest, op_add)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DynDenseMatrix<Real> n(2, 2);
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    auto o = m + n;
    EXPECT_EQ(o(0, 0), 2.);
    EXPECT_EQ(o(0, 1), -3.);
    EXPECT_EQ(o(1, 0), 2.);
    EXPECT_EQ(o(1, 1), -3.);
}

TEST(DynDenseMatrixTest, op_inc_mat)
{
    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, { 1, 1, 0 });
    m.set_row(1, { 1, 1, 1 });
    m.set_row(2, { 0, 1, 1 });

    DynDenseMatrix<Real> n(3, 3);
    n.set_row(0, { 2, -1, 1 });
    n.set_row(1, { 1, -3, 4 });
    n.set_row(2, { -1, -1, -2 });

    m += n;
    EXPECT_EQ(m(0, 0), 3.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), -2.);
    EXPECT_EQ(m(1, 2), 5.);
    EXPECT_EQ(m(2, 0), -1.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), -1.);
}

TEST(DynDenseMatrixTest, subtract)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DynDenseMatrix<Real> n(2, 2);
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    m.subtract(n);
    EXPECT_EQ(m(0, 0), 0.);
    EXPECT_EQ(m(0, 1), -1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(1, 1), 3.);
}

TEST(DynDenseMatrixTest, op_subtract)
{
    DynDenseMatrix<Real> m(2, 2);
    m.set_row(0, { 1., -2. });
    m.set_row(1, { 2., 0. });
    DynDenseMatrix<Real> n(2, 2);
    n.set_row(0, { 1., -1. });
    n.set_row(1, { 0., -3. });
    auto o = m - n;
    EXPECT_EQ(o(0, 0), 0.);
    EXPECT_EQ(o(0, 1), -1.);
    EXPECT_EQ(o(1, 0), 2.);
    EXPECT_EQ(o(1, 1), 3.);
}

TEST(DynDenseMatrixTest, op_mult_scalar)
{
    DynDenseMatrix<Real> m(3, 3);
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

TEST(DynDenseMatrixTest, op_mult_scalar_pre)
{
    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, { 2, 1, 0 });
    m.set_row(1, { 1, 2, -1 });
    m.set_row(2, { 0, -1, 2 });
    auto res = 3. * m;
    EXPECT_EQ(res.get_num_cols(), 3);
    EXPECT_EQ(res.get_num_rows(), 3);
    EXPECT_EQ(res(0, 0), 6.);
    EXPECT_EQ(res(0, 1), 3.);
    EXPECT_EQ(res(0, 2), 0.);
    EXPECT_EQ(res(1, 0), 3.);
    EXPECT_EQ(res(1, 1), 6.);
    EXPECT_EQ(res(1, 2), -3.);
    EXPECT_EQ(res(2, 0), 0.);
    EXPECT_EQ(res(2, 1), -3.);
    EXPECT_EQ(res(2, 2), 6.);
}

TEST(DynDenseMatrixTest, op_mult)
{
    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, { 1, 1, 0 });
    m.set_row(1, { 1, 1, 1 });
    m.set_row(2, { 0, 1, 1 });
    DynDenseVector<Real> v(3);
    v.set_values({ 2., 3., 4. });

    auto res = m * v;
    EXPECT_EQ(res(0), 5.);
    EXPECT_EQ(res(1), 9.);
    EXPECT_EQ(res(2), 7.);
}

TEST(DynDenseMatrixTest, create_diagonal)
{
    auto m = DynDenseMatrix<Real>::create_diagonal({ 1, 2, 3 });
    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), 0.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), 3.);
}

TEST(DynDenseMatrixTest, transpose3)
{
    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, { 2, -3, 4 });
    m.set_row(1, { -5, 6, -7 });
    m.set_row(2, { 8, -1, -2 });
    auto tr = transpose(m);
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

TEST(DynDenseMatrixTest, mult_mat)
{
    auto a = DynDenseMatrix<Real>(2, 3);
    a.set_row(0, { 2, -3, 4 });
    a.set_row(1, { -5, 6, -7 });
    auto b = DynDenseMatrix<Real>(3, 4);
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

TEST(DynDenseMatrixTest, set_row)
{
    DynDenseMatrix<Real> m(3, 2);
    m.zero();

    m.set_row(0, { 1, 3 });
    m.set_row(1, { 2, 5 });
    m.set_row(2, { 0, 4 });

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(2, 1), 4.);
}

TEST(DynDenseMatrixTest, set_row_dense_vec)
{
    DynDenseVector<Real> row0(3);
    row0.set_values({ 2, 1, 7 });

    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, row0);
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(0, 2), 7.);
}

TEST(DynDenseMatrixTest, set_row_dense_mat)
{
    DynDenseMatrix<Real> row0(1, 3);
    row0.set_row(0, { 2, 1, 7 });

    DynDenseMatrix<Real> m(3, 3);
    m.set_row(0, row0);
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(0, 2), 7.);
}

TEST(DynDenseMatrixTest, set_col)
{
    DynDenseMatrix<Real> m(3, 2);
    m.zero();

    m.set_col(0, { 1, 2, 0 });
    m.set_col(1, { 3, 5, 4 });

    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(1, 0), 2.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(2, 1), 4.);
}

TEST(DynDenseMatrixTest, set_col_dense_vec)
{
    std::vector<Real> col0 = { 2, 1, 7 };

    DynDenseMatrix<Real> m(3, 3);
    m.set_col(1, col0);
    EXPECT_EQ(m(0, 1), 2.);
    EXPECT_EQ(m(1, 1), 1.);
    EXPECT_EQ(m(2, 1), 7.);
}

TEST(DynDenseMatrixTest, out)
{
    testing::internal::CaptureStdout();

    DynDenseMatrix<Real> m(2, 3);
    m.set_row(0, { 1, 2, 3 });
    m.set_row(1, { 6, 5, 4 });

    std::cout << m;

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("(1, 2, 3)"));
    EXPECT_THAT(out, HasSubstr("(6, 5, 4)"));
}

TEST(DynDenseMatrixTest, assign)
{
    DynDenseMatrix<Real> m(2, 3);
    m.set_row(0, { 1, 2, 3 });
    m.set_row(1, { 6, 5, 4 });
    DynDenseMatrix<Real> b(2, 3);
    b = m;
    EXPECT_EQ(m.get_num_rows(), 2);
    EXPECT_EQ(m.get_num_cols(), 3);
    EXPECT_DOUBLE_EQ(b(0, 0), 1.);
    EXPECT_DOUBLE_EQ(b(0, 1), 2.);
    EXPECT_DOUBLE_EQ(b(0, 2), 3.);
    EXPECT_DOUBLE_EQ(b(1, 0), 6.);
    EXPECT_DOUBLE_EQ(b(1, 1), 5.);
    EXPECT_DOUBLE_EQ(b(1, 2), 4.);
}

TEST(DynDenseMatrixTest, assign_into_empty)
{
    DynDenseMatrix<Real> m(2, 3);
    m.set_row(0, { 1, 2, 3 });
    m.set_row(1, { 6, 5, 4 });
    DynDenseMatrix<Real> b;
    b = m;
    EXPECT_EQ(m.get_num_rows(), 2);
    EXPECT_EQ(m.get_num_cols(), 3);
    EXPECT_DOUBLE_EQ(b(0, 0), 1.);
    EXPECT_DOUBLE_EQ(b(0, 1), 2.);
    EXPECT_DOUBLE_EQ(b(0, 2), 3.);
    EXPECT_DOUBLE_EQ(b(1, 0), 6.);
    EXPECT_DOUBLE_EQ(b(1, 1), 5.);
    EXPECT_DOUBLE_EQ(b(1, 2), 4.);
}
