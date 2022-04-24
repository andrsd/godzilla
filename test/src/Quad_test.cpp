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
