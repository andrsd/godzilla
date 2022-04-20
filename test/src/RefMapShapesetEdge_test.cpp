#include "Common.h"
#include "RefMapShapeset1D.h"
#include "QuadratureGauss1D.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(RefMapShapesetEdgeTest, base)
{
    RefMapShapesetEdge ss;
    EXPECT_EQ(ss.get_num_components(), 1);
    EXPECT_EQ(ss.get_vertex_index(0), 0);
    EXPECT_EQ(ss.get_vertex_index(1), 1);

    EXPECT_EQ(ss.get_bubble_indices(2), nullptr);

    EXPECT_EQ(ss.get_order(0), 1);
    EXPECT_EQ(ss.get_order(1), 1);

    EXPECT_EQ(ss.get_num_bubble_fns(2), 0);
    EXPECT_EQ(ss.get_num_bubble_fns(3), 0);
}

TEST(RefMapShapesetEdgeTest, vertex_fns)
{
    RefMapShapesetEdge ss;
    uint idx;

    idx = ss.get_vertex_index(0);
    EXPECT_DOUBLE_EQ(ss.get_fn_value(idx, 0.5, 0), 0.25);
    EXPECT_DOUBLE_EQ(ss.get_dx_value(idx, 0.5, 0), -0.5);

    idx = ss.get_vertex_index(1);
    EXPECT_DOUBLE_EQ(ss.get_fn_value(idx, 0.5, 0), 0.75);
    EXPECT_DOUBLE_EQ(ss.get_dx_value(idx, 0.5, 0), 0.5);
}

TEST(RefMapShapesetEdgeTest, qpt)
{
    uint qorder = 2;
    QuadratureGauss1D q;
    auto n_pts = q.get_num_points(qorder);
    auto qpts = q.get_points(qorder);

    RefMapShapesetEdge ss;
    auto idx = ss.get_vertex_index(0);
    Real vals[n_pts];

    ss.get_fn_values(idx, n_pts, qpts, 0, vals);
    EXPECT_DOUBLE_EQ(vals[0], 0.78867513459481497);
    EXPECT_DOUBLE_EQ(vals[1], 0.21132486540518503);

    ss.get_dx_values(idx, n_pts, qpts, 0, vals);
    EXPECT_DOUBLE_EQ(vals[0], -0.5);
    EXPECT_DOUBLE_EQ(vals[1], -0.5);
}
