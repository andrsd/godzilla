#include "GodzillaConfig.h"
#include "Error.h"
#include "CallStack.h"
#include "Edge.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(EdgeTest, ctor)
{
    Edge edge_empty;

    Index vs[] = { 1, 2 };
    Edge edge_1(vs);

    Edge edge_2(1, 2);

    Edge edge_copy(edge_1);
}

TEST(EdgeTest, id)
{
    Edge edge;
    EXPECT_EQ(edge.get_id(), INVALID_IDX);
}

TEST(EdgeTest, marker)
{
    Edge edge;
    edge.set_marker(1234);
    EXPECT_EQ(edge.get_marker(), 1234);
}
