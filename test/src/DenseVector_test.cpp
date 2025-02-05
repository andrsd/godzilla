#include "gmock/gmock.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/DenseMatrixSymm.h"

using namespace godzilla;
using namespace testing;

TEST(DenseVectorTest, ctor)
{
    DenseVector<Real, 3> v;
    for (Int i = 0; i < 3; ++i)
        v(i) = 10. + (Real) i;
    for (Int i = 0; i < 3; ++i)
        EXPECT_DOUBLE_EQ(v(i), 10 + i);
}

TEST(DenseVectorTest, ctor_std_vector)
{
    std::vector<Real> a = { 2., 3., 4. };
    DenseVector<Real, 3> v(a);
    EXPECT_DOUBLE_EQ(v(0), 2.);
    EXPECT_DOUBLE_EQ(v(1), 3.);
    EXPECT_DOUBLE_EQ(v(2), 4.);
}

TEST(DenseVectorTest, ctor_std_array)
{
    DenseVector<Real, 3> v({ 2., 3., 4. });
    EXPECT_DOUBLE_EQ(v(0), 2.);
    EXPECT_DOUBLE_EQ(v(1), 3.);
    EXPECT_DOUBLE_EQ(v(2), 4.);
}

TEST(DenseVectorTest, ctor_init)
{
    DenseVector<Real, 3> v { 2., 3., 4. };
    EXPECT_DOUBLE_EQ(v(0), 2.);
    EXPECT_DOUBLE_EQ(v(1), 3.);
    EXPECT_DOUBLE_EQ(v(2), 4.);
}

TEST(DenseVectorTest, assign_std_array)
{
    DenseVector<Real, 3> v;
    v = { 2., 3., 4. };
    EXPECT_DOUBLE_EQ(v(0), 2.);
    EXPECT_DOUBLE_EQ(v(1), 3.);
    EXPECT_DOUBLE_EQ(v(2), 4.);
}

TEST(DenseVectorTest, zero)
{
    DenseVector<Real, 3> a({ 1, 2, 3 });
    a.zero();
    EXPECT_DOUBLE_EQ(a(0), 0.);
    EXPECT_DOUBLE_EQ(a(1), 0.);
    EXPECT_DOUBLE_EQ(a(2), 0.);
}

TEST(DenseVectorTest, set_values)
{
    DenseVector<Real, 3> a;
    a.set_values(2.);
    EXPECT_DOUBLE_EQ(a(0), 2.);
    EXPECT_DOUBLE_EQ(a(1), 2.);
    EXPECT_DOUBLE_EQ(a(2), 2.);
}

TEST(DenseVectorTest, scale)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    a.scale(3.);
    EXPECT_DOUBLE_EQ(a(0), 6.);
    EXPECT_DOUBLE_EQ(a(1), 9.);
    EXPECT_DOUBLE_EQ(a(2), 12.);
}

TEST(DenseVectorTest, add)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    a.add(b);
    EXPECT_DOUBLE_EQ(a(0), 6.);
    EXPECT_DOUBLE_EQ(a(1), 5.);
    EXPECT_DOUBLE_EQ(a(2), 3.);
}

TEST(DenseVectorTest, add_scalar)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    a.add(2);
    EXPECT_DOUBLE_EQ(a(0), 4.);
    EXPECT_DOUBLE_EQ(a(1), 5.);
    EXPECT_DOUBLE_EQ(a(2), 6.);
}

TEST(DenseVectorTest, op_add)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    DenseVector<Real, 3> c = a + b;
    EXPECT_DOUBLE_EQ(c(0), 6.);
    EXPECT_DOUBLE_EQ(c(1), 5.);
    EXPECT_DOUBLE_EQ(c(2), 3.);
}

TEST(DenseVectorTest, subtract)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    a.subtract(b);
    EXPECT_DOUBLE_EQ(a(0), -2.);
    EXPECT_DOUBLE_EQ(a(1), 1.);
    EXPECT_DOUBLE_EQ(a(2), 5.);
}

TEST(DenseVectorTest, op_subtract)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    DenseVector<Real, 3> c = a - b;
    EXPECT_DOUBLE_EQ(c(0), -2.);
    EXPECT_DOUBLE_EQ(c(1), 1.);
    EXPECT_DOUBLE_EQ(c(2), 5.);
}

TEST(DenseVectorTest, dot_vec_vec)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    EXPECT_DOUBLE_EQ(dot(a, b), 10.);
}

TEST(DenseVectorTest, dot_row_vec)
{
    DenseMatrix<Real, 1, 3> a;
    a.set_row(0, { 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    EXPECT_DOUBLE_EQ(dot(a, b), 10.);
}

TEST(DenseVectorTest, dot_row_row)
{
    DenseMatrix<Real, 1, 3> a;
    a.set_row(0, { 2., 3., 4. });
    DenseMatrix<Real, 1, 3> b;
    b.set_row(0, { 4., 2., -1. });
    EXPECT_DOUBLE_EQ(dot(a, b), 10.);
}

TEST(DenseVectorTest, dot_1x1)
{
    DenseMatrix<Real, 1, 1> a;
    a(0, 0) = 2.;
    DenseMatrix<Real, 1, 1> b;
    b(0, 0) = 3.;
    EXPECT_DOUBLE_EQ(dot(a, b), 6.);
}

TEST(DenseVectorTest, sum)
{
    DenseVector<Real, 3> b({ 4., 2., -1. });
    Real sum = b.sum();
    EXPECT_DOUBLE_EQ(sum, 5.);
}

TEST(DenseVectorTest, magnitude)
{
    DenseVector<Real, 2> v({ 3., 4. });
    Real mag = v.magnitude();
    EXPECT_DOUBLE_EQ(mag, 5.);
}

TEST(DenseVectorTest, magnitude_vec)
{
    DenseVector<Real, 2> v({ 3., 4. });
    Real mag = magnitude(v);
    EXPECT_DOUBLE_EQ(mag, 5.);
}

TEST(DenseVectorTest, pointwise_mult_vec)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 3., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    DenseVector<Real, N> res = pointwise_mult(a, b);
    EXPECT_DOUBLE_EQ(res(0), 8.);
    EXPECT_DOUBLE_EQ(res(1), 6.);
    EXPECT_DOUBLE_EQ(res(2), -4.);
}

TEST(DenseVectorTest, pointwise_mult_row)
{
    const Int N = 3;
    DenseMatrix<Real, 1, N> a;
    a.set_row(0, { 2., 3., 4. });
    DenseMatrix<Real, 1, N> b;
    b.set_row(0, { 4., 2., -1. });
    DenseMatrix<Real, 1, N> res = pointwise_mult(a, b);
    EXPECT_DOUBLE_EQ(res(0, 0), 8.);
    EXPECT_DOUBLE_EQ(res(0, 1), 6.);
    EXPECT_DOUBLE_EQ(res(0, 2), -4.);
}

TEST(DenseVectorTest, pointwise_div_col_col)
{
    const Int N = 3;
    DenseVector<Real, N> a({ 2., 6., 4. });
    DenseVector<Real, N> b({ 4., 2., -1. });
    DenseVector<Real, N> res = pointwise_div<Real, N>(a, b);
    EXPECT_DOUBLE_EQ(res(0), 0.5);
    EXPECT_DOUBLE_EQ(res(1), 3.);
    EXPECT_DOUBLE_EQ(res(2), -4.);
}

TEST(DenseVectorTest, pointwise_div_row_row)
{
    const Int N = 3;
    DenseMatrix<Real, 1, N> a;
    a.set_row(0, { 2., 6., 4. });
    DenseMatrix<Real, 1, N> b;
    b.set_row(0, { 4., 2., -1. });
    auto res = pointwise_div(a, b);
    EXPECT_DOUBLE_EQ(res(0, 0), 0.5);
    EXPECT_DOUBLE_EQ(res(0, 1), 3.);
    EXPECT_DOUBLE_EQ(res(0, 2), -4.);
}

TEST(DenseVectorTest, op_mult_scalar)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b = 3. * a;
    EXPECT_DOUBLE_EQ(b(0), 6.);
    EXPECT_DOUBLE_EQ(b(1), 9.);
    EXPECT_DOUBLE_EQ(b(2), 12.);
}

TEST(DenseVectorTest, op_mult_scalar_post)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b = a * 3.;
    EXPECT_DOUBLE_EQ(b(0), 6.);
    EXPECT_DOUBLE_EQ(b(1), 9.);
    EXPECT_DOUBLE_EQ(b(2), 12.);
}

TEST(DenseVectorTest, op_mult_row_mat)
{
    DenseVector<Real, 3> a({ 1, 2, 3 });
    DenseMatrix<Real, 1, 2> b;
    b.set_row(0, { 5, 7 });
    auto prod = a * b;
    EXPECT_DOUBLE_EQ(prod(0, 0), 5.);
    EXPECT_DOUBLE_EQ(prod(1, 0), 10.);
    EXPECT_DOUBLE_EQ(prod(2, 0), 15.);
    EXPECT_DOUBLE_EQ(prod(0, 1), 7.);
    EXPECT_DOUBLE_EQ(prod(1, 1), 14.);
    EXPECT_DOUBLE_EQ(prod(2, 1), 21.);
}

#ifndef NDEBUG

TEST(DenseVectorDeathTest, set_out_of_range)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    EXPECT_DEATH({ a(4) = 1.; }, "Assertion.+failed");
}

TEST(DenseVectorDeathTest, get_out_of_range)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    const DenseVector<Real, 3> & ca = a;
    EXPECT_DEATH({ [[maybe_unused]] auto r = ca(4); }, "Assertion.+failed");
}

#endif

TEST(DenseVectorTest, cross_prod_3)
{
    DenseVector<Real, 3> a({ -2., 5, 1. });
    DenseVector<Real, 3> b({ 3, 1, 2 });
    auto v = cross_product(a, b);
    EXPECT_DOUBLE_EQ(v(0), 9.);
    EXPECT_DOUBLE_EQ(v(1), 7.);
    EXPECT_DOUBLE_EQ(v(2), -17.);
}

TEST(DenseVectorTest, mat_row)
{
    DenseVector<DenseVector<Real, 3>, 2> a;
    a(0) = DenseVector<Real, 3>({ 1, 2, 3 });
    a(1) = DenseVector<Real, 3>({ -3, -4, -5 });
    auto mat = mat_row(a);
    EXPECT_DOUBLE_EQ(mat(0, 0), 1.);
    EXPECT_DOUBLE_EQ(mat(0, 1), 2.);
    EXPECT_DOUBLE_EQ(mat(0, 2), 3.);
    EXPECT_DOUBLE_EQ(mat(1, 0), -3.);
    EXPECT_DOUBLE_EQ(mat(1, 1), -4.);
    EXPECT_DOUBLE_EQ(mat(1, 2), -5.);
}

TEST(DenseVectorTest, mat_col)
{
    DenseVector<DenseVector<Real, 3>, 2> a;
    a(0) = DenseVector<Real, 3>({ 1, 2, 3 });
    a(1) = DenseVector<Real, 3>({ -3, -4, -5 });
    auto mat = mat_col(a);
    EXPECT_DOUBLE_EQ(mat(0, 0), 1.);
    EXPECT_DOUBLE_EQ(mat(0, 1), -3.);
    EXPECT_DOUBLE_EQ(mat(1, 0), 2.);
    EXPECT_DOUBLE_EQ(mat(1, 1), -4.);
    EXPECT_DOUBLE_EQ(mat(2, 0), 3.);
    EXPECT_DOUBLE_EQ(mat(2, 1), -5.);
}

TEST(DenseVectorTest, avg)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    EXPECT_DOUBLE_EQ(a.avg(), 2.);
}

TEST(DenseVectorTest, op_inc)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    DenseVector<Real, 3> b({ 1., -1, 2. });
    a += b;
    EXPECT_DOUBLE_EQ(a(0), -1.);
    EXPECT_DOUBLE_EQ(a(1), 4.);
    EXPECT_DOUBLE_EQ(a(2), 5.);
}

TEST(DenseVectorTest, op_inc_scalar)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    a += 2.;
    EXPECT_DOUBLE_EQ(a(0), 0.);
    EXPECT_DOUBLE_EQ(a(1), 7.);
    EXPECT_DOUBLE_EQ(a(2), 5.);
}

TEST(DenseVectorTest, op_dec)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    DenseVector<Real, 3> b({ 1., -1, 2. });
    a -= b;
    EXPECT_DOUBLE_EQ(a(0), -3.);
    EXPECT_DOUBLE_EQ(a(1), 6.);
    EXPECT_DOUBLE_EQ(a(2), 1.);
}

TEST(DenseVectorTest, op_unary_minus)
{
    DenseVector<Real, 3> a({ -2., 5, 3. });
    auto b = -a;
    EXPECT_DOUBLE_EQ(b(0), 2.);
    EXPECT_DOUBLE_EQ(b(1), -5.);
    EXPECT_DOUBLE_EQ(b(2), -3.);
}

TEST(DenseVectorTest, min)
{
    DenseVector<Real, 3> a({ 5, -2, 10 });
    EXPECT_DOUBLE_EQ(a.min(), -2);
}

TEST(DenseVectorTest, max)
{
    DenseVector<Real, 3> a({ 5, -2, 10 });
    EXPECT_DOUBLE_EQ(a.max(), 10);
}

TEST(DenseVectorTest, abs)
{
    DenseVector<Real, 3> v({ -5, 2, -10 });
    v.abs();
    EXPECT_DOUBLE_EQ(v(0), 5.);
    EXPECT_DOUBLE_EQ(v(1), 2.);
    EXPECT_DOUBLE_EQ(v(2), 10.);
}

TEST(DenseVectorTest, normalize)
{
    DenseVector<Real, 2> v({ 3., 4. });
    v.normalize();
    EXPECT_DOUBLE_EQ(v(0), 3. / 5.);
    EXPECT_DOUBLE_EQ(v(1), 4. / 5.);
}

TEST(DenseVectorTest, normalized)
{
    DenseVector<Real, 2> v({ 3., 4. });
    auto n = v.normalized();
    EXPECT_DOUBLE_EQ(n(0), 3. / 5.);
    EXPECT_DOUBLE_EQ(n(1), 4. / 5.);
}

TEST(DenseVectorTest, out)
{
    testing::internal::CaptureStdout();

    DenseVector<Real, 3> v({ 1, 2, 3 });

    std::cout << v;

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("(1, 2, 3)"));
}

TEST(DenseVectorTest, tensor_product)
{
    DenseMatrix<Real, 3, 1> a;
    a.set_col(0, { 1, 2, 3 });
    DenseMatrix<Real, 1, 2> b;
    b.set_row(0, { 5, 7 });
    auto prod = a * b;
    EXPECT_DOUBLE_EQ(prod(0, 0), 5.);
    EXPECT_DOUBLE_EQ(prod(1, 0), 10.);
    EXPECT_DOUBLE_EQ(prod(2, 0), 15.);
    EXPECT_DOUBLE_EQ(prod(0, 1), 7.);
    EXPECT_DOUBLE_EQ(prod(1, 1), 14.);
    EXPECT_DOUBLE_EQ(prod(2, 1), 21.);
}

// --

TEST(DynDenseVectorTest, ctor_empty)
{
    DynDenseVector<Real> v;
    EXPECT_DOUBLE_EQ(v.size(), 0);
}

TEST(DynDenseVectorTest, ctor)
{
    DynDenseVector<Real> v(3);
    EXPECT_DOUBLE_EQ(v.size(), 3);
    for (Int i = 0; i < 3; ++i)
        v(i) = 10. + (Real) i;
    for (Int i = 0; i < 3; ++i)
        EXPECT_DOUBLE_EQ(v(i), 10 + i);
}

TEST(DynDenseVectorTest, ctor_init)
{
    DynDenseVector<Real> v(3, 123.);
    for (Int i = 0; i < 3; ++i)
        EXPECT_DOUBLE_EQ(v(i), 123.);
}

TEST(DynDenseVectorTest, ctor_std_vector)
{
    DynDenseVector<Real> v(3);
    v.set_values({ 2., 3., 4. });
    EXPECT_DOUBLE_EQ(v(0), 2.);
    EXPECT_DOUBLE_EQ(v(1), 3.);
    EXPECT_DOUBLE_EQ(v(2), 4.);
}

TEST(DynDenseVectorTest, zero)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 1, 2, 3 });
    a.zero();
    EXPECT_DOUBLE_EQ(a(0), 0.);
    EXPECT_DOUBLE_EQ(a(1), 0.);
    EXPECT_DOUBLE_EQ(a(2), 0.);
}

TEST(DynDenseVectorTest, set_values)
{
    DynDenseVector<Real> a(3);
    a.set_values(2.);
    EXPECT_DOUBLE_EQ(a(0), 2.);
    EXPECT_DOUBLE_EQ(a(1), 2.);
    EXPECT_DOUBLE_EQ(a(2), 2.);
}

TEST(DynDenseVectorTest, scale)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    a.scale(3.);
    EXPECT_DOUBLE_EQ(a(0), 6.);
    EXPECT_DOUBLE_EQ(a(1), 9.);
    EXPECT_DOUBLE_EQ(a(2), 12.);
}

TEST(DynDenseVectorTest, add)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    a.add(b);
    EXPECT_DOUBLE_EQ(a(0), 6.);
    EXPECT_DOUBLE_EQ(a(1), 5.);
    EXPECT_DOUBLE_EQ(a(2), 3.);
}

TEST(DynDenseVectorTest, add_scalar)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    a.add(2);
    EXPECT_DOUBLE_EQ(a(0), 4.);
    EXPECT_DOUBLE_EQ(a(1), 5.);
    EXPECT_DOUBLE_EQ(a(2), 6.);
}

TEST(DynDenseVectorTest, op_add)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    DynDenseVector<Real> c = a + b;
    EXPECT_DOUBLE_EQ(c(0), 6.);
    EXPECT_DOUBLE_EQ(c(1), 5.);
    EXPECT_DOUBLE_EQ(c(2), 3.);
}

TEST(DynDenseVectorTest, subtract)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    a.subtract(b);
    EXPECT_DOUBLE_EQ(a(0), -2.);
    EXPECT_DOUBLE_EQ(a(1), 1.);
    EXPECT_DOUBLE_EQ(a(2), 5.);
}

TEST(DynDenseVectorTest, op_subtract)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    DynDenseVector<Real> c = a - b;
    EXPECT_DOUBLE_EQ(c(0), -2.);
    EXPECT_DOUBLE_EQ(c(1), 1.);
    EXPECT_DOUBLE_EQ(c(2), 5.);
}

TEST(DynDenseVectorTest, dot_vec_vec)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    EXPECT_DOUBLE_EQ(dot(a, b), 10.);
}

TEST(DynDenseVectorTest, sum)
{
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    Real sum = b.sum();
    EXPECT_DOUBLE_EQ(sum, 5.);
}

TEST(DynDenseVectorTest, magnitude)
{
    DynDenseVector<Real> v(2);
    v.set_values({ 3., 4. });
    Real mag = v.magnitude();
    EXPECT_DOUBLE_EQ(mag, 5.);
}

TEST(DynDenseVectorTest, pointwise_mult)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    DynDenseVector<Real> res = pointwise_mult(a, b);
    EXPECT_DOUBLE_EQ(res(0), 8.);
    EXPECT_DOUBLE_EQ(res(1), 6.);
    EXPECT_DOUBLE_EQ(res(2), -4.);
}

TEST(DynDenseVectorTest, pointwise_div)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 6., 4. });
    DynDenseVector<Real> b(3);
    b.set_values({ 4., 2., -1. });
    DynDenseVector<Real> res = pointwise_div(a, b);
    EXPECT_DOUBLE_EQ(res(0), 0.5);
    EXPECT_DOUBLE_EQ(res(1), 3.);
    EXPECT_DOUBLE_EQ(res(2), -4.);
}

TEST(DynDenseVectorTest, op_mult_scalar)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b = 3. * a;
    EXPECT_DOUBLE_EQ(b(0), 6.);
    EXPECT_DOUBLE_EQ(b(1), 9.);
    EXPECT_DOUBLE_EQ(b(2), 12.);
}

TEST(DynDenseVectorTest, op_mult_scalar_post)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 2., 3., 4. });
    DynDenseVector<Real> b = a * 3.;
    EXPECT_DOUBLE_EQ(b(0), 6.);
    EXPECT_DOUBLE_EQ(b(1), 9.);
    EXPECT_DOUBLE_EQ(b(2), 12.);
}

TEST(DynDenseVectorTest, cross_prod_3)
{
    DynDenseVector<Real> a(3);
    a.set_values({ -2., 5, 1. });
    DynDenseVector<Real> b(3);
    b.set_values({ 3, 1, 2 });
    auto v = cross_product(a, b);
    EXPECT_DOUBLE_EQ(v(0), 9.);
    EXPECT_DOUBLE_EQ(v(1), 7.);
    EXPECT_DOUBLE_EQ(v(2), -17.);
}

TEST(DynDenseVectorTest, avg)
{
    DynDenseVector<Real> a(3);
    a.set_values({ -2., 5, 3. });
    EXPECT_DOUBLE_EQ(a.avg(), 2.);
}

TEST(DynDenseVectorTest, op_inc)
{
    DynDenseVector<Real> a(3);
    a.set_values({ -2., 5, 3. });
    DynDenseVector<Real> b(3);
    b.set_values({ 1., -1, 2. });
    a += b;
    EXPECT_DOUBLE_EQ(a(0), -1.);
    EXPECT_DOUBLE_EQ(a(1), 4.);
    EXPECT_DOUBLE_EQ(a(2), 5.);
}

TEST(DynDenseVectorTest, op_inc_scalar)
{
    DynDenseVector<Real> a(3);
    a.set_values({ -2., 5, 3. });
    a += 2.;
    EXPECT_DOUBLE_EQ(a(0), 0.);
    EXPECT_DOUBLE_EQ(a(1), 7.);
    EXPECT_DOUBLE_EQ(a(2), 5.);
}

TEST(DynDenseVectorTest, op_dec)
{
    DynDenseVector<Real> a(3);
    a.set_values({ -2., 5, 3. });
    DynDenseVector<Real> b(3);
    b.set_values({ 1., -1, 2. });
    a -= b;
    EXPECT_DOUBLE_EQ(a(0), -3.);
    EXPECT_DOUBLE_EQ(a(1), 6.);
    EXPECT_DOUBLE_EQ(a(2), 1.);
}

TEST(DynDenseVectorTest, op_unary_minus)
{
    DynDenseVector<Real> a(3);
    a.set_values({ -2., 5, 3. });
    auto b = -a;
    EXPECT_DOUBLE_EQ(b(0), 2.);
    EXPECT_DOUBLE_EQ(b(1), -5.);
    EXPECT_DOUBLE_EQ(b(2), -3.);
}

TEST(DynDenseVectorTest, min)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 5, -2, 10 });
    EXPECT_DOUBLE_EQ(a.min(), -2);
}

TEST(DynDenseVectorTest, max)
{
    DynDenseVector<Real> a(3);
    a.set_values({ 5, -2, 10 });
    EXPECT_DOUBLE_EQ(a.max(), 10);
}

TEST(DynDenseVectorTest, abs)
{
    DynDenseVector<Real> v(3);
    v.set_values({ -5, 2, -10 });
    v.abs();
    EXPECT_DOUBLE_EQ(v(0), 5.);
    EXPECT_DOUBLE_EQ(v(1), 2.);
    EXPECT_DOUBLE_EQ(v(2), 10.);
}

TEST(DynDenseVectorTest, normalize)
{
    DynDenseVector<Real> v(2);
    v.set_values({ 3., 4. });
    v.normalize();
    EXPECT_DOUBLE_EQ(v(0), 3. / 5.);
    EXPECT_DOUBLE_EQ(v(1), 4. / 5.);
}

TEST(DynDenseVectorTest, normalized)
{
    DynDenseVector<Real> v(2);
    v.set_values({ 3., 4. });
    auto n = v.normalized();
    EXPECT_DOUBLE_EQ(n(0), 3. / 5.);
    EXPECT_DOUBLE_EQ(n(1), 4. / 5.);
}

TEST(DynDenseVectorTest, resize)
{
    DynDenseVector<Real> v(2);
    v.set_values({ 3., 4. });
    v.resize(3);
    EXPECT_DOUBLE_EQ(v.size(), 3);
}

TEST(DynDenseVectorTest, out)
{
    testing::internal::CaptureStdout();

    DynDenseVector<Real> v(3);
    v.set_values({ 1, 2, 3 });

    std::cout << v;

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("(1, 2, 3)"));
}
