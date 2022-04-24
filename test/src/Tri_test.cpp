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
