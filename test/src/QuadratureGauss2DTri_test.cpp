#include "QuadratureGauss2DTri.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(QuadratureGauss2DTriTest, test)
{
    QuadratureGauss2DTri q;
    uint p_order = 2;

    auto np = q.get_num_points(p_order);
    EXPECT_EQ(np, 3);

    auto pts = q.get_points(p_order);
    const QPoint2D & qp = pts[0];
    EXPECT_DOUBLE_EQ(qp.x, -2.0 / 3);
    EXPECT_DOUBLE_EQ(qp.y, -2.0 / 3);
    EXPECT_DOUBLE_EQ(qp.w, 2. / 3.);

    auto edge_pts = q.get_edge_points(0, p_order);

    auto order = q.get_max_order();
    EXPECT_EQ(order, 20);
}

TEST(QuadratureGauss2DTriTest, idx)
{
    QuadratureGauss2DTri & q = QuadratureGauss2DTri::get();
    uint p_order = 2;

    auto pts = q.get_points(p_order);
    const QPoint2D & qp = pts[0];
    EXPECT_DOUBLE_EQ(qp[0], -2.0 / 3);
    EXPECT_DOUBLE_EQ(qp[1], -2.0 / 3);
}

TEST(QuadratureGauss2DTriTest, invalid_idx)
{
    QuadratureGauss2DTri & q = QuadratureGauss2DTri::get();
    uint p_order = 2;

    auto pts = q.get_points(p_order);
    const QPoint2D & qp = pts[0];
    EXPECT_DEATH(qp[3], "Index out of bounds");
}
