#include "QuadratureGauss1D.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(QuadratureGauss1DTest, test)
{
    QuadratureGauss1D q;
    uint p_order = 2;

    auto np = q.get_num_points(p_order);
    EXPECT_EQ(np, 2);

    auto pts = q.get_points(p_order);
    const QPoint1D & qp = pts[0];
    EXPECT_DOUBLE_EQ(qp.x, -0.57735026918963);
    EXPECT_DOUBLE_EQ(qp.w, 1.);

    auto order = q.get_max_order();
    EXPECT_EQ(order, 24);
}

TEST(QuadratureGauss1DTest, idx)
{
    QuadratureGauss1D & q = QuadratureGauss1D::get();
    uint p_order = 2;

    auto pts = q.get_points(p_order);
    const QPoint1D & qp = pts[0];
    EXPECT_DOUBLE_EQ(qp[0], -0.57735026918963);
}

TEST(QuadratureGauss1DTest, invalid_idx)
{
    QuadratureGauss1D & q = QuadratureGauss1D::get();
    uint p_order = 2;

    auto pts = q.get_points(p_order);
    const QPoint1D & qp = pts[0];
    EXPECT_DEATH(qp[1], "Index out of bounds");
}
