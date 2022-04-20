#include "Common.h"
#include "H1LobattoShapesetEdge.h"
#include "QuadratureGauss1D.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(H1LobattoShapesetEdgeTest, base)
{
    H1LobattoShapesetEdge ss;
    EXPECT_EQ(ss.get_num_components(), 1);
    EXPECT_EQ(ss.get_vertex_index(0), 0);
    EXPECT_EQ(ss.get_vertex_index(1), 1);

    EXPECT_EQ(ss.get_order(0), 1);
    EXPECT_EQ(ss.get_order(1), 1);
    EXPECT_EQ(ss.get_order(2), 2);
    EXPECT_EQ(ss.get_order(3), 3);

    EXPECT_EQ(ss.get_num_bubble_fns(2), 1);
    EXPECT_EQ(ss.get_num_bubble_fns(3), 2);
}

TEST(H1LobattoShapesetEdgeTest, vertex_fns)
{
    H1LobattoShapesetEdge ss;
    uint idx;

    idx = ss.get_vertex_index(0);
    EXPECT_DOUBLE_EQ(ss.get_fn_value(idx, 0.5, 0), 0.25);
    EXPECT_DOUBLE_EQ(ss.get_dx_value(idx, 0.5, 0), -0.5);

    idx = ss.get_vertex_index(1);
    EXPECT_DOUBLE_EQ(ss.get_fn_value(idx, 0.5, 0), 0.75);
    EXPECT_DOUBLE_EQ(ss.get_dx_value(idx, 0.5, 0), 0.5);
}

TEST(H1LobattoShapesetEdgeTest, bubble_fns)
{
    H1LobattoShapesetEdge ss;
    uint idx;
    uint * bub_idx = ss.get_bubble_indices(3);

    idx = bub_idx[0];
    EXPECT_DOUBLE_EQ(ss.get_fn_value(idx, 0.5, 0), -0.45927932677184585);
    EXPECT_DOUBLE_EQ(ss.get_dx_value(idx, 0.5, 0), 0.61237243569579447);

    idx = bub_idx[1];
    EXPECT_DOUBLE_EQ(ss.get_fn_value(idx, 0.5, 0), -0.29646353064078557);
    EXPECT_DOUBLE_EQ(ss.get_dx_value(idx, 0.5, 0), -0.19764235376052372);
}

TEST(H1LobattoShapesetEdgeTest, qpt)
{
    uint order = 2;
    QuadratureGauss1D q;
    auto n_pts = q.get_num_points(order);
    auto qpts = q.get_points(order);

    H1LobattoShapesetEdge ss;
    auto idx = ss.get_vertex_index(0);
    Real vals[n_pts];

    ss.get_fn_values(idx, n_pts, qpts, 0, vals);
    EXPECT_DOUBLE_EQ(vals[0], 0.78867513459481497);
    EXPECT_DOUBLE_EQ(vals[1], 0.21132486540518503);

    ss.get_dx_values(idx, n_pts, qpts, 0, vals);
    EXPECT_DOUBLE_EQ(vals[0], -0.5);
    EXPECT_DOUBLE_EQ(vals[1], -0.5);
}
