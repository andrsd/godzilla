#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Tetra.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(TetraTest, ctor)
{
    Tetra tet_empty;

    PetscInt vs[] = { 1, 2, 3, 4 };
    Tetra tet_1(vs);

    Tetra tet_2(1, 2, 3, 4);

    Tetra tet_copy(tet_1);
}

TEST(TetraTest, id)
{
    Tetra tet;
    EXPECT_EQ(tet.get_id(), INVALID_IDX);
}

TEST(TetraTest, marker)
{
    Tetra tet;
    tet.set_marker(1234);
    EXPECT_EQ(tet.get_marker(), 1234);
}

TEST(TetraTest, geom)
{
    Tetra tet;
    EXPECT_EQ(tet.get_num_vertices(), 4);
    EXPECT_EQ(tet.get_num_edges(), 6);
    EXPECT_EQ(tet.get_num_faces(), 4);
}
