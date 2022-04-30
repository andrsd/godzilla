#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Tri.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(TriTest, ctor)
{
    Tri tet_empty(0);

    PetscInt vs[] = { 1, 2, 3 };
    Tri tri_1(1, vs);

    Tri tri_2(2, 1, 2, 3);

    Tri tet_copy(tri_1);
}

TEST(TriTest, id)
{
    Tri tet(123);
    EXPECT_EQ(tet.get_id(), 123);
}

TEST(TriTest, marker)
{
    Tri tet(0);
    tet.set_marker(1234);
    EXPECT_EQ(tet.get_marker(), 1234);
}

TEST(TriTest, geom)
{
    Tri tri(0);
    EXPECT_EQ(tri.get_num_vertices(), 3);
    EXPECT_EQ(tri.get_num_edges(), 3);
}

TEST(TriTest, vtcs)
{
    Tri tri(2, 1, 2, 3);
    const PetscInt * vtcs = tri.get_vertices();
    EXPECT_EQ(vtcs[0], 1);
    EXPECT_EQ(vtcs[1], 2);
    EXPECT_EQ(vtcs[2], 3);

    for (uint i = 0; i < Tri::NUM_VERTICES; i++)
        EXPECT_EQ(tri.get_vertex(i), i + 1);
}

TEST(TriTest, polytope_type)
{
    Tri tri(0);
    EXPECT_EQ(tri.get_potytope_type(), DM_POLYTOPE_TRIANGLE);
}

TEST(TriTest, edge_oris)
{
    Tri tri(0);
    EXPECT_DEATH(tri.get_edge_orientation(0), "Not implemented");
}

TEST(TriTest, face_oris)
{
    Tri tri(0);
    EXPECT_DEATH(tri.get_face_orientation(0), "2D triangle has no face functions");
}
