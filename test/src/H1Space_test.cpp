#include "GodzillaApp_test.h"
#include "Common.h"
#include "LineMesh.h"
#include "Edge.h"
#include "H1LobattoShapesetEdge.h"
#include "H1Space.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(H1SpaceTest, first_order_1d)
{
    TestApp app;

    InputParameters params = LineMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<Real>("xmax") = 2.;
    params.set<uint>("nx") = 2;
    LineMesh mesh(params);

    mesh.create();

    H1LobattoShapesetEdge ss;

    H1Space sp(&mesh, &ss);
    sp.set_uniform_order(2);

    sp.assign_dofs();
    EXPECT_EQ(sp.get_dof_count(), 5);
}
