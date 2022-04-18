#include "GodzillaConfig.h"
#include "Error.h"
#include "CallStack.h"
#include "Hex.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(HexTest, ctor)
{
    Hex hex_empty;

    Index vs[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    Hex hex_1(vs);

    Hex hex_2(1, 2, 3, 4, 5, 6, 7, 8);

    Hex hex_copy(hex_1);
}

TEST(HexTest, id)
{
    Hex hex;
    EXPECT_EQ(hex.get_id(), INVALID_IDX);
}

TEST(HexTest, marker)
{
    Hex hex;
    hex.set_marker(1234);
    EXPECT_EQ(hex.get_marker(), 1234);
}
