#include "gmock/gmock.h"
#include "DenseVector.h"
#include "DenseMatrix.h"

using namespace godzilla;

TEST(DenseVectorTest, ctor)
{
    DenseVector<Real, 3> v;
    for (Int i = 0; i < 3; i++)
        v(i) = 10. + (Real) i;
    for (Int i = 0; i < 3; i++)
        EXPECT_EQ(v(i), 10 + i);
}

TEST(DenseVectorTest, ctor_std_vector)
{
    DenseVector<Real, 3> v({ 2., 3., 4. });
    EXPECT_EQ(v(0), 2.);
    EXPECT_EQ(v(1), 3.);
    EXPECT_EQ(v(2), 4.);
}

TEST(DenseVectorTest, zero)
{
    DenseVector<Real, 3> a({ 1, 2, 3 });
    a.zero();
    EXPECT_EQ(a(0), 0.);
    EXPECT_EQ(a(1), 0.);
    EXPECT_EQ(a(2), 0.);
}

TEST(DenseVectorTest, set_values)
{
    DenseVector<Real, 3> a;
    a.set_values(2.);
    EXPECT_EQ(a(0), 2.);
    EXPECT_EQ(a(1), 2.);
    EXPECT_EQ(a(2), 2.);
}

TEST(DenseVectorTest, scale)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    a.scale(3.);
    EXPECT_EQ(a(0), 6.);
    EXPECT_EQ(a(1), 9.);
    EXPECT_EQ(a(2), 12.);
}

TEST(DenseVectorTest, add)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    a.add(b);
    EXPECT_EQ(a(0), 6.);
    EXPECT_EQ(a(1), 5.);
    EXPECT_EQ(a(2), 3.);
}

TEST(DenseVectorTest, add_scalar)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    a.add(2);
    EXPECT_EQ(a(0), 4.);
    EXPECT_EQ(a(1), 5.);
    EXPECT_EQ(a(2), 6.);
}

TEST(DenseVectorTest, op_add)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    DenseVector<Real, 3> c = a + b;
    EXPECT_EQ(c(0), 6.);
    EXPECT_EQ(c(1), 5.);
    EXPECT_EQ(c(2), 3.);
}

TEST(DenseVectorTest, subtract)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    a.subtract(b);
    EXPECT_EQ(a(0), -2.);
    EXPECT_EQ(a(1), 1.);
    EXPECT_EQ(a(2), 5.);
}

TEST(DenseVectorTest, op_subtract)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    DenseVector<Real, 3> c = a - b;
    EXPECT_EQ(c(0), -2.);
    EXPECT_EQ(c(1), 1.);
    EXPECT_EQ(c(2), 5.);
}

TEST(DenseVectorTest, dot)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    Real dot = a.dot(b);
    EXPECT_EQ(dot, 10.);
}

TEST(DenseVectorTest, dot_glob)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 3., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    Real dot_prod = dot<Real, N>(a, b);
    EXPECT_EQ(dot_prod, 10.);
}

TEST(DenseVectorTest, sum)
{
    DenseVector<Real, 3> b({ 4., 2., -1. });
    Real sum = b.sum();
    EXPECT_EQ(sum, 5.);
}

TEST(DenseVectorTest, magnitude)
{
    DenseVector<Real, 2> v({ 3., 4. });
    Real mag = v.magnitude();
    EXPECT_EQ(mag, 5.);
}

TEST(DenseVectorTest, pointwise_mult)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 3., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    DenseVector<Real, N> res = a.pointwise_mult(b);
    EXPECT_EQ(res(0), 8.);
    EXPECT_EQ(res(1), 6.);
    EXPECT_EQ(res(2), -4.);
}

TEST(DenseVectorTest, pointwise_mult_glob)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 3., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    DenseVector<Real, N> res = pointwise_mult<Real, N>(a, b);
    EXPECT_EQ(res(0), 8.);
    EXPECT_EQ(res(1), 6.);
    EXPECT_EQ(res(2), -4.);
}

TEST(DenseVectorTest, pointwise_div)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 6., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    DenseVector<Real, N> res = a.pointwise_div(b);
    EXPECT_EQ(res(0), 0.5);
    EXPECT_EQ(res(1), 3.);
    EXPECT_EQ(res(2), -4.);
}

TEST(DenseVectorTest, pointwise_div_glob)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 6., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    DenseVector<Real, N> res = pointwise_div<Real, N>(a, b);
    EXPECT_EQ(res(0), 0.5);
    EXPECT_EQ(res(1), 3.);
    EXPECT_EQ(res(2), -4.);
}

TEST(DenseVectorTest, op_mult_mat)
{
    DenseVector<Real, 2> a({ 2., 3. });
    DenseMatrix<Real, 2, 3> b;
    b.set_row(0, { 4., 2., -1. });
    b.set_row(1, { 0., -3., 1. });
    DenseVector<Real, 3> x = a * b;
    EXPECT_EQ(x(0), 8.);
    EXPECT_EQ(x(1), -5.);
    EXPECT_EQ(x(2), 1.);
}

TEST(DenseVectorTest, op_mult_vecvec)
{
    DenseVector<Real, 2> a({ 2., 3. });
    DenseVector<DenseVector<Real, 2>, 3> b;
    b(0)(0) = 4.;
    b(0)(1) = 0.;
    b(1)(0) = 2.;
    b(1)(1) = -3.;
    b(2)(0) = -1.;
    b(2)(1) = 1.;
    DenseVector<Real, 3> x = a * b;
    EXPECT_EQ(x(0), 8.);
    EXPECT_EQ(x(1), -5.);
    EXPECT_EQ(x(2), 1.);
}

TEST(DenseVectorTest, op_mult_vec)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    Real dot = a * b;
    EXPECT_EQ(dot, 10.);
}

TEST(DenseVectorTest, op_mult_scalar)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b = 3. * a;
    EXPECT_EQ(b(0), 6.);
    EXPECT_EQ(b(1), 9.);
    EXPECT_EQ(b(2), 12.);
}

TEST(DenseVectorTest, op_mult_scalar_post)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b = a * 3.;
    EXPECT_EQ(b(0), 6.);
    EXPECT_EQ(b(1), 9.);
    EXPECT_EQ(b(2), 12.);
}

TEST(DenseVectorDeathTest, set_out_of_range)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    EXPECT_DEATH({ a(4) = 1.; }, "Assertion.+failed");
}

TEST(DenseVectorDeathTest, get_out_of_range)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    const DenseVector<Real, 3> & ca = a;
    EXPECT_DEATH({ auto r = ca(4); }, "Assertion.+failed");
}

TEST(DenseVectorTest, tensor_prod)
{
    DenseVector<Real, 3> a({ -2., 5., 1. });
    DenseVector<Real, 2> b({ 3, 1 });
    auto m = a.tensor_prod(b);
    EXPECT_EQ(m(0, 0), -6.);
    EXPECT_EQ(m(0, 1), -2.);
    EXPECT_EQ(m(1, 0), 15.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(2, 0), 3.);
    EXPECT_EQ(m(2, 1), 1.);
}

TEST(DenseVectorDeathTest, cross_prod_1_2)
{
    DenseVector<Real, 1> a({ -2. });
    EXPECT_DEATH(a.cross(a), "Cross product of 1D vectors is not defined.");

    DenseVector<Real, 2> b({ -2., 5. });
    EXPECT_DEATH(b.cross(b), "Cross product of 2D vectors is not defined.");

    DenseVector<Real, 4> c({ -2., 5., 5., 6. });
    EXPECT_DEATH(c.cross(c), "Cross product in 4 dimensions is not unique.");
}

TEST(DenseVectorTest, cross_prod_3)
{
    DenseVector<Real, 3> a({ -2., 5, 1. });
    DenseVector<Real, 3> b({ 3, 1, 2 });
    auto v = a.cross(b);
    EXPECT_EQ(v(0), 9.);
    EXPECT_EQ(v(1), 7.);
    EXPECT_EQ(v(2), -17.);
}

TEST(DenseVectorTest, avg)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    EXPECT_EQ(a.avg(), 2.);
}

TEST(DenseVectorTest, op_inc)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    DenseVector<Real, 3> b({ 1., -1, 2. });
    a += b;
    EXPECT_EQ(a(0), -1.);
    EXPECT_EQ(a(1), 4.);
    EXPECT_EQ(a(2), 5.);
}

TEST(DenseVectorTest, op_inc_scalar)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    a += 2.;
    EXPECT_EQ(a(0), 0.);
    EXPECT_EQ(a(1), 7.);
    EXPECT_EQ(a(2), 5.);
}

TEST(DenseVectorTest, op_dec)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    DenseVector<Real, 3> b({ 1., -1, 2. });
    a -= b;
    EXPECT_EQ(a(0), -3.);
    EXPECT_EQ(a(1), 6.);
    EXPECT_EQ(a(2), 1.);
}

TEST(DenseVectorTest, op_unary_minus)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    auto b = -a;
    EXPECT_EQ(b(0), 2.);
    EXPECT_EQ(b(1), -5.);
    EXPECT_EQ(b(2), -3.);
}

TEST(DenseVectorTest, mat_row)
{
    DenseVector<DenseVector<Real, 3>, 2> A;
    A(0)(0) = -2;
    A(0)(1) = 5;
    A(0)(2) = 3;
    A(1)(0) = 1;
    A(1)(1) = -1;
    A(1)(2) = 2;

    DenseMatrix<Real, 2, 3> m = mat_row(A);
    EXPECT_EQ(m(0, 0), -2.);
    EXPECT_EQ(m(0, 1), 5.);
    EXPECT_EQ(m(0, 2), 3.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), -1.);
    EXPECT_EQ(m(1, 2), 2.);
}

TEST(DenseVectorTest, mat_col)
{
    DenseVector<DenseVector<Real, 3>, 2> A;
    A(0)(0) = -2;
    A(0)(1) = 5;
    A(0)(2) = 3;
    A(1)(0) = 1;
    A(1)(1) = -1;
    A(1)(2) = 2;

    DenseMatrix<Real, 3, 2> m = mat_col(A);

    EXPECT_EQ(m(0, 0), -2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(1, 0), 5.);
    EXPECT_EQ(m(1, 1), -1.);
    EXPECT_EQ(m(2, 0), 3.);
    EXPECT_EQ(m(2, 1), 2.);
}

TEST(DenseVectorTest, min)
{
    DenseVector<Real, 3> a({ 5, -2, 10 });
    EXPECT_EQ(a.min(), -2);
}

TEST(DenseVectorTest, max)
{
    DenseVector<Real, 3> a({ 5, -2, 10 });
    EXPECT_EQ(a.max(), 10);
}

TEST(DenseVectorTest, transpose)
{
    DenseVector<DenseVector<Real, 3>, 2> A;
    A(0)(0) = -2;
    A(0)(1) = 5;
    A(0)(2) = 3;
    A(1)(0) = 1;
    A(1)(1) = -1;
    A(1)(2) = 2;

    DenseVector<DenseVector<Real, 2>, 3> m = transpose(A);
    EXPECT_EQ(m(0)(0), -2.);
    EXPECT_EQ(m(0)(1), 1.);
    EXPECT_EQ(m(1)(0), 5.);
    EXPECT_EQ(m(1)(1), -1.);
    EXPECT_EQ(m(2)(0), 3.);
    EXPECT_EQ(m(2)(1), 2.);
}

TEST(DenseVectorTest, abs)
{
    DenseVector<Real, 3> v({ -5, 2, -10 });
    v.abs();
    EXPECT_EQ(v(0), 5.);
    EXPECT_EQ(v(1), 2.);
    EXPECT_EQ(v(2), 10.);
}
