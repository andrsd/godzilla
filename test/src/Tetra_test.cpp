#include "GodzillaConfig.h"
#include "Error.h"
#include "CallStack.h"
#include "Tetra.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(TetraTest, ctor)
{
    Tetra tet_empty;

    Index vs[] = { 1, 2, 3, 4 };
    Tetra tet_1(vs);

    Tetra tet_2(1, 2, 3, 4);

    Tetra tet_copy(tet_1);
}

TEST(TetraTest, id)
{
    Tetra tet;
    EXPECT_EQ(tet.getId(), INVALID_IDX);
}

TEST(TetraTest, marker)
{
    Tetra tet;
    tet.setMarker(1234);
    EXPECT_EQ(tet.getMarker(), 1234);
}
