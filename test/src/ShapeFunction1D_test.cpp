#include "GodzillaApp_test.h"
#include "Common.h"
#include "LineMesh.h"
#include "Edge.h"
#include "H1LobattoShapesetEdge.h"
#include "ShapeFunction1D.h"
#include "QuadratureGauss1D.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(ShapeFunction1DTest, base)
{
    TestApp app;

    InputParameters params = LineMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<uint>("nx") = 1;
    LineMesh mesh(params);

    mesh.create();

    const Element * e = mesh.get_element(0);
    const Edge * edge = dynamic_cast<const Edge *>(e);
    assert(edge != nullptr);

    H1LobattoShapesetEdge ss;

    ShapeFunction1D sh_fn(&ss);
    EXPECT_EQ(sh_fn.get_shapeset(), &ss);

    sh_fn.set_active_element(edge);
    EXPECT_EQ(sh_fn.get_active_element(), edge);

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
