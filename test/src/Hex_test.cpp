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
