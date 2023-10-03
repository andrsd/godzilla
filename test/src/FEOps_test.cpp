#include "gmock/gmock.h"
#include "FEOps.h"

using namespace godzilla;

TEST(FEOpsTest, lin_comb)
{
    DenseVector<Real, 2> a({ 2., 3. });
    DenseVector<DenseVector<Real, 3>, 2> b;
    b(0)(0) = -1;
    b(0)(1) = 1;
    b(0)(2) = -2;
    b(1)(0) = 2;
    b(1)(1) = 1;
    b(1)(2) = 0;
    auto lc = fe::linear_combination(a, b);

    EXPECT_EQ(lc(0), 4.);
    EXPECT_EQ(lc(1), 5.);
    EXPECT_EQ(lc(2), -4.);
}

TEST(FEOpsTest, gradient_scalar)
{
    DenseVector<Real, 2> a({ 2., 3. });
    DenseMatrix<Real, 2, 3> grad_phi;
    grad_phi.set_row(0, { -1, 1, -2 });
    grad_phi.set_row(1, { 2, 1, 0 });

    auto grad_a = fe::gradient(a, grad_phi);
    EXPECT_EQ(grad_a(0), 4.);
    EXPECT_EQ(grad_a(1), 5.);
    EXPECT_EQ(grad_a(2), -4.);
}

TEST(FEOpsTest, gradient_vector)
{
    DenseVector<DenseVector<Real, 3>, 2> f;
    f(0) = DenseVector<Real, 3>({ 3, -1, 2 });
    f(1) = DenseVector<Real, 3>({ -2, 1, -3 });

    DenseMatrix<Real, 2, 2> grad_phi;
    grad_phi.set_row(0, { -1, -2 });
    grad_phi.set_row(1, { 2, 0 });

    auto grad_F = fe::gradient(f, grad_phi);
    EXPECT_DOUBLE_EQ(grad_F(0, 0), -7.);
    EXPECT_DOUBLE_EQ(grad_F(0, 1), 3.);
    EXPECT_DOUBLE_EQ(grad_F(0, 2), -8.);
    EXPECT_DOUBLE_EQ(grad_F(1, 0), -6.);
    EXPECT_DOUBLE_EQ(grad_F(1, 1), 2.);
    EXPECT_DOUBLE_EQ(grad_F(1, 2), -4.);
}

TEST(FEOpsTest, gradient_vector_mat)
{
    DenseMatrix<Real, 2, 3> f;
    f.set_row(0, { 3, -1, 2 });
    f.set_row(1, { -2, 1, -3 });

    DenseMatrix<Real, 2, 2> grad_phi;
    grad_phi.set_row(0, { -1, -2 });
    grad_phi.set_row(1, { 2, 0 });

    auto grad_F = fe::gradient(f, grad_phi);
    EXPECT_DOUBLE_EQ(grad_F(0, 0), -7.);
    EXPECT_DOUBLE_EQ(grad_F(0, 1), 3.);
    EXPECT_DOUBLE_EQ(grad_F(0, 2), -8.);
    EXPECT_DOUBLE_EQ(grad_F(1, 0), -6.);
    EXPECT_DOUBLE_EQ(grad_F(1, 1), 2.);
    EXPECT_DOUBLE_EQ(grad_F(1, 2), -4.);
}
