#include "gmock/gmock.h"
#include "Types.h"

using namespace godzilla;

TEST(TypesTest, get_element_type_str)
{
    EXPECT_EQ(get_element_type_str(EDGE2), "EDGE2");
    EXPECT_EQ(get_element_type_str(TRI3), "TRI3");
    EXPECT_EQ(get_element_type_str(QUAD4), "QUAD4");
    EXPECT_EQ(get_element_type_str(TET4), "TET4");
    EXPECT_EQ(get_element_type_str(HEX8), "HEX8");
}

TEST(TypesTest, get_num_element_nodes)
{
    EXPECT_EQ(get_num_element_nodes(EDGE2), 2);
    EXPECT_EQ(get_num_element_nodes(TRI3), 3);
    EXPECT_EQ(get_num_element_nodes(QUAD4), 4);
    EXPECT_EQ(get_num_element_nodes(TET4), 4);
    EXPECT_EQ(get_num_element_nodes(HEX8), 8);
}
