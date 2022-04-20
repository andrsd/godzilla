#include "Common.h"
#include "Mesh.h"
#include "Edge.h"
#include "H1LobattoShapesetEdge.h"
#include "ShapeFunction1D.h"
#include "QuadratureGauss1D.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(ShapeFunction1DTest, base)
{
    Vertex1D v1(0.);
    Vertex1D v2(1.);

    Mesh mesh;
    mesh.set_vertex(0, &v1);
    mesh.set_vertex(1, &v2);

    Edge edge(0, 1);
    mesh.set_element(0, &edge);

    H1LobattoShapesetEdge ss;

    ShapeFunction1D sh_fn(&ss);
    EXPECT_EQ(sh_fn.get_shapeset(), &ss);

    sh_fn.set_active_element(&edge);
    EXPECT_EQ(sh_fn.get_active_element(), &edge);

    sh_fn.set_active_shape(0);
    EXPECT_EQ(sh_fn.get_active_shape(), 0);
    EXPECT_EQ(sh_fn.get_fn_order(), 1);
    EXPECT_EQ(sh_fn.get_num_components(), 1);

    QuadratureGauss1D quad;
    uint qorder = 2;
    uint np = quad.get_num_points(qorder);
    QPoint1D * qpts = quad.get_points(qorder);

    sh_fn.precalculate(np, qpts, RealFunction1D::FN_DEFAULT);

    Real * val = sh_fn.get_fn_values();
    EXPECT_DOUBLE_EQ(val[0], 0.78867513459481497);
    EXPECT_DOUBLE_EQ(val[1], 0.21132486540518503);

    Real * dx = sh_fn.get_dx_values();
    EXPECT_DOUBLE_EQ(dx[0], -0.5);
    EXPECT_DOUBLE_EQ(dx[1], -0.5);
}
