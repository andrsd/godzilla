#include <gmock/gmock.h>
#include "TestApp.h"
#include "godzilla/StructuredMesh.h"

using namespace godzilla;

TEST(StructuredMeshTest, get_corners_1d)
{
    TestApp app;

    auto da = StructuredMesh::create_1d(app.get_comm(), DM_BOUNDARY_NONE, 12, 1, 1);
    da.set_up();

    auto corners = da.get_corners<1_D>();
    EXPECT_EQ(corners.index[0], 0);
    EXPECT_EQ(corners.width[0], 12);
}

TEST(StructuredMeshTest, get_corners_2d)
{
    TestApp app;

    auto da = StructuredMesh::create_2d(app.get_comm(),
                                        DM_BOUNDARY_NONE,
                                        DM_BOUNDARY_NONE,
                                        DMDA_STENCIL_STAR,
                                        12,
                                        5,
                                        PETSC_DECIDE,
                                        PETSC_DECIDE,
                                        1,
                                        1);
    da.set_up();

    auto corners = da.get_corners<2_D>();
    EXPECT_EQ(corners.index[0], 0);
    EXPECT_EQ(corners.index[1], 0);
    EXPECT_EQ(corners.width[0], 12);
    EXPECT_EQ(corners.width[1], 5);
}
