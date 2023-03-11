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

TEST(DenseVectorTest, set_val)
{
    DenseVector<Real, 3> a;
    a.set_val(2.);
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

TEST(DenseVectorTest, op_add)
{
    DenseVector<Real, 3> a({ 2., 3., 4. });
    DenseVector<Real, 3> b({ 4., 2., -1. });
    DenseVector<Real, 3> c = a + b;
    EXPECT_EQ(c(0), 6.);
    EXPECT_EQ(c(1), 5.);
    EXPECT_EQ(c(2), 3.);
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
