#include "Common.h"
#include "Mesh.h"
#include "Edge.h"
#include "H1LobattoShapesetEdge.h"
#include "H1Space.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(H1SpaceTest, first_order_1d)
{
    Vertex1D vtcs[] = { Vertex1D(0.), Vertex1D(1.), Vertex1D(2.) };
    Mesh mesh;
    for (uint i = 0; i < countof(vtcs); i++)
        mesh.set_vertex(i, &vtcs[i]);
    for (uint i = 0; i < countof(vtcs) - 1; i++) {
        Edge * edge = new Edge(i, i + 1);
        mesh.set_element(i, edge);
    }
    // mesh.set_boundary(0, 0, 1);
    // mesh.set_boundary(1, 1, 2);
    mesh.set_dimension(1);
    mesh.create();

    H1LobattoShapesetEdge ss;

    H1Space sp(&mesh, &ss);
    sp.set_uniform_order(1);

    sp.assign_dofs();
    // EXPECT_EQ(sp.get_dof_count(), 3);
}
