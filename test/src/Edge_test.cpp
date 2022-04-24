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
