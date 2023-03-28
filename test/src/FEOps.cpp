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

TEST(FEOpsTest, grad_u_grad_phi)
{
    DenseVector<Real, 2> grad_u({ 2., 3. });
    DenseVector<DenseVector<Real, 2>, 3> grad_phi;
    grad_phi(0)(0) = -1;
    grad_phi(0)(1) = 1;
    grad_phi(1)(0) = 1;
    grad_phi(1)(1) = -1;
    grad_phi(2)(0) = 2;
    grad_phi(2)(1) = 0;
    auto l = fe::grad_u_grad_phi(grad_u, grad_phi);
    EXPECT_EQ(l(0), 1.);
    EXPECT_EQ(l(1), -1.);
    EXPECT_EQ(l(2), 4.);
}
