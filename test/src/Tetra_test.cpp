#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Tetra.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(TetraTest, ctor)
{
    Tetra tet_empty(0);

    PetscInt vs[] = { 1, 2, 3, 4 };
    Tetra tet_1(1, vs);

    Tetra tet_2(2, 1, 2, 3, 4);

    Tetra tet_copy(tet_1);
}

TEST(TetraTest, id)
{
    Tetra tet(123);
    EXPECT_EQ(tet.get_id(), 123);
}

TEST(TetraTest, marker)
{
    Tetra tet(0);
    tet.set_marker(1234);
    EXPECT_EQ(tet.get_marker(), 1234);
}

TEST(TetraTest, geom)
{
    Tetra tet(0);
    EXPECT_EQ(tet.get_num_vertices(), 4);
    EXPECT_EQ(tet.get_num_edges(), 6);
    EXPECT_EQ(tet.get_num_faces(), 4);
}
