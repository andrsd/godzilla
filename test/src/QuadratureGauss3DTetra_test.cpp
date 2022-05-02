#include "QuadratureGauss3DTetra.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(QuadratureGauss3DTetraTest, test)
{
    QuadratureGauss3DTetra q;
    uint p_order = 2;

    auto np = q.get_num_points(p_order);
    EXPECT_EQ(np, 4);
    auto edge_np = q.get_edge_num_points(0, p_order);
    EXPECT_EQ(edge_np, 2);
    auto face_np = q.get_face_num_points(0, p_order);
    EXPECT_EQ(face_np, 3);

    auto pts = q.get_points(p_order);
    const QPoint3D & qp = pts[0];
    EXPECT_DOUBLE_EQ(qp.x, -0.7236067977499790);
    EXPECT_DOUBLE_EQ(qp.y, -0.7236067977499790);
    EXPECT_DOUBLE_EQ(qp.z, -0.7236067977499790);
    EXPECT_DOUBLE_EQ(qp.w, 0.3333333333333330);

    auto edge_pts = q.get_edge_points(0, p_order);
    EXPECT_DOUBLE_EQ(edge_pts[0].x, -0.57735026918962995);
    EXPECT_DOUBLE_EQ(edge_pts[0].y, -1.);
    EXPECT_DOUBLE_EQ(edge_pts[0].z, -1.);
    EXPECT_DOUBLE_EQ(edge_pts[0].w, 1.);

    auto face_pts = q.get_face_points(0, p_order);
    EXPECT_DOUBLE_EQ(face_pts[0].x, -0.66666666666666663);
    EXPECT_DOUBLE_EQ(face_pts[0].y, -1.);
    EXPECT_DOUBLE_EQ(face_pts[0].z, -0.66666666666666663);
    EXPECT_DOUBLE_EQ(face_pts[0].w, 0.66666666666666663);

    auto edge_order = q.get_edge_max_order(0);
    EXPECT_EQ(edge_order, 20);
    auto face_order = q.get_face_max_order(0);
    EXPECT_EQ(face_order, 20);
    auto order = q.get_max_order();
    EXPECT_EQ(order, 20);
}

TEST(QuadratureGauss3DTetraTest, idx)
{
    QuadratureGauss3DTetra & q = QuadratureGauss3DTetra::get();
    uint p_order = 2;

    auto pts = q.get_points(p_order);
    const QPoint3D & qp = pts[0];
    EXPECT_DOUBLE_EQ(qp[0], -0.7236067977499790);
    EXPECT_DOUBLE_EQ(qp[1], -0.7236067977499790);
    EXPECT_DOUBLE_EQ(qp[2], -0.7236067977499790);
}

TEST(QuadratureGauss3DTetraTest, invalid_idx)
{
    QuadratureGauss3DTetra & q = QuadratureGauss3DTetra::get();
    uint p_order = 2;

    auto pts = q.get_points(p_order);
    const QPoint3D & qp = pts[0];
    EXPECT_DEATH(qp[3], "Index out of bounds");
}
