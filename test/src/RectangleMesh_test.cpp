#include "GodzillaApp_test.h"
#include "RectangleMesh_test.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

registerObject(MockRectangleMesh);

TEST_F(RectangleMeshTest, g2d_rectangle_mesh)
{
    auto obj = g2dRectangleMesh({ 1, 2 }, { 3, 4 });

    EXPECT_EQ(obj->getXMin(), 1);
    EXPECT_EQ(obj->getXMax(), 3);
    EXPECT_EQ(obj->getNx(), 9);

    EXPECT_EQ(obj->getYMin(), 2);
    EXPECT_EQ(obj->getYMax(), 4);
    EXPECT_EQ(obj->getNy(), 8);
}

TEST_F(RectangleMeshTest, g2d_rectangle_mesh_incorrect_dims)
{
    testing::internal::CaptureStderr();

    g2dRectangleMesh({ 2, 1 }, { 1, 2 });
    g2dRectangleMesh({ 1, 2 }, { 2, 1 });
    this->app->checkIntegrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
}

TEST_F(RectangleMeshTest, g2d_rectangle_mesh_create)
{
    auto obj = g2dRectangleMesh({ 1, 2 }, { 3, 4 });
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 2);

    PetscReal gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 3);

    EXPECT_EQ(gmin[1], 2);
    EXPECT_EQ(gmax[1], 4);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    PetscInt n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 180);
}
