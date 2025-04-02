#include "gmock/gmock.h"
#include "godzilla/Types.h"

using namespace godzilla;

TEST(TypesTest, get_num_element_nodes)
{
    EXPECT_EQ(get_num_element_nodes(EDGE2), 2);
    EXPECT_EQ(get_num_element_nodes(TRI3), 3);
    EXPECT_EQ(get_num_element_nodes(QUAD4), 4);
    EXPECT_EQ(get_num_element_nodes(TET4), 4);
    EXPECT_EQ(get_num_element_nodes(HEX8), 8);
}
