#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Quad.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(QuadTest, ctor)
{
    Quad tet_empty(0);

    PetscInt vs[] = { 1, 2, 3, 4 };
    Quad quad_1(1, vs);

    Quad quad_2(2, 1, 2, 3, 4);

    Quad quad_copy(quad_1);
}

TEST(QuadTest, id)
{
    Quad quad(123);
    EXPECT_EQ(quad.get_id(), 123);
}

TEST(QuadTest, marker)
{
    Quad quad(0);
    quad.set_marker(1234);
    EXPECT_EQ(quad.get_marker(), 1234);
}

TEST(QuadTest, geom)
{
    Quad quad(0);
    EXPECT_EQ(quad.get_num_vertices(), 4);
    EXPECT_EQ(quad.get_num_edges(), 4);
}

TEST(QuadTest, vtcs)
{
    Quad quad(2, 1, 2, 3, 4);
    const PetscInt * vtcs = quad.get_vertices();
    EXPECT_EQ(vtcs[0], 1);
    EXPECT_EQ(vtcs[1], 2);
    EXPECT_EQ(vtcs[2], 3);
    EXPECT_EQ(vtcs[3], 4);

    for (uint i = 0; i < Quad::NUM_VERTICES; i++)
        EXPECT_EQ(quad.get_vertex(i), i + 1);
}

TEST(QuadTest, polytope_type)
{
    Quad quad(0);
    EXPECT_EQ(quad.get_potytope_type(), DM_POLYTOPE_QUADRILATERAL);
}

TEST(QuadTest, edge_oris)
{
    Quad quad(0);
    EXPECT_DEATH(quad.get_edge_orientation(0), "Not implemented");
}

TEST(QuadTest, face_oris)
{
    Quad quad(0);
    EXPECT_DEATH(quad.get_face_orientation(0), "2D quadrilateral has no face functions");
}
