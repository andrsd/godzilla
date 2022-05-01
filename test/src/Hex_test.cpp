#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Hex.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(HexTest, ctor)
{
    Hex hex_empty(0);

    PetscInt vs[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    Hex hex_1(1, vs);

    Hex hex_2(2, 1, 2, 3, 4, 5, 6, 7, 8);

    Hex hex_copy(hex_1);
}

TEST(HexTest, id)
{
    Hex hex(123);
    EXPECT_EQ(hex.get_id(), 123);
}

TEST(HexTest, marker)
{
    Hex hex(1);
    hex.set_marker(1234);
    EXPECT_EQ(hex.get_marker(), 1234);
}

TEST(HexTest, geom)
{
    Hex hex(1);
    EXPECT_EQ(hex.get_num_vertices(), 8);
    EXPECT_EQ(hex.get_num_edges(), 12);
    EXPECT_EQ(hex.get_num_faces(), 6);
}

TEST(HexTest, vtcs)
{
    Hex hex(2, 1, 2, 3, 4, 5, 6, 7, 8);
    const PetscInt * vtcs = hex.get_vertices();
    EXPECT_EQ(vtcs[0], 1);
    EXPECT_EQ(vtcs[1], 2);
    EXPECT_EQ(vtcs[2], 3);
    EXPECT_EQ(vtcs[3], 4);
    EXPECT_EQ(vtcs[4], 5);
    EXPECT_EQ(vtcs[5], 6);
    EXPECT_EQ(vtcs[6], 7);
    EXPECT_EQ(vtcs[7], 8);

    for (uint i = 0; i < Hex::NUM_VERTICES; i++)
        EXPECT_EQ(hex.get_vertex(i), i + 1);
}

TEST(HexTest, polytope_type)
{
    Hex hex(0);
    EXPECT_EQ(hex.get_potytope_type(), DM_POLYTOPE_HEXAHEDRON);
}

TEST(HexTest, edge_oris)
{
    Hex hex(0);
    EXPECT_DEATH(hex.get_edge_orientation(0), "Not implemented");
}

TEST(HexTest, face_oris)
{
    Hex hex(0);
    EXPECT_DEATH(hex.get_face_orientation(0), "Not implemented");
}
