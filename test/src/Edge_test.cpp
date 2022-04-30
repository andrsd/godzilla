#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Edge.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(EdgeTest, ctor)
{
    Edge edge_empty(0);

    PetscInt vs[] = { 1, 2 };
    Edge edge_1(1, vs);

    Edge edge_2(2, 1, 2);

    Edge edge_copy(edge_1);
}

TEST(EdgeTest, id)
{
    Edge edge(123);
    EXPECT_EQ(edge.get_id(), 123);
}

TEST(EdgeTest, marker)
{
    Edge edge(0);
    edge.set_marker(1234);
    EXPECT_EQ(edge.get_marker(), 1234);
}

TEST(EdgeTest, geom)
{
    Edge edge(0);
    EXPECT_EQ(edge.get_num_vertices(), 2);
}

TEST(EdgeTest, vtcs)
{
    Edge edge(0, 3, 4);
    const PetscInt * vtcs = edge.get_vertices();
    EXPECT_EQ(vtcs[0], 3);
    EXPECT_EQ(vtcs[1], 4);
}

TEST(EdgeTest, polytope_type)
{
    Edge edge(0);
    EXPECT_EQ(edge.get_potytope_type(), DM_POLYTOPE_SEGMENT);
}

TEST(EdgeTest, edge_oris)
{
    Edge edge(0);
    EXPECT_DEATH(edge.get_edge_orientation(0), "1D edge has no edge functions");
}

TEST(EdgeTest, face_oris)
{
    Edge edge(0);
    EXPECT_DEATH(edge.get_face_orientation(0), "1D edge has no face functions");
}
