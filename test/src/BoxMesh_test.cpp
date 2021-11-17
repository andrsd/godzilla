#include "GodzillaApp_test.h"
#include "BoxMesh_test.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

registerObject(MockBoxMesh);

TEST_F(BoxMeshTest, g2d_rectangle_mesh)
{
    auto obj = g3dBoxMesh({ 1, 2, 3 }, { 4, 5, 6 });

    EXPECT_EQ(obj->getXMin(), 1);
    EXPECT_EQ(obj->getXMax(), 4);
    EXPECT_EQ(obj->getNx(), 9);

    EXPECT_EQ(obj->getYMin(), 2);
    EXPECT_EQ(obj->getYMax(), 5);
    EXPECT_EQ(obj->getNy(), 8);

    EXPECT_EQ(obj->getZMin(), 3);
    EXPECT_EQ(obj->getZMax(), 6);
    EXPECT_EQ(obj->getNz(), 7);
}

TEST_F(BoxMeshTest, g3d_rectangle_mesh_incorrect_dims)
{
    EXPECT_DEATH(g3dBoxMesh({ 2, 1, 0 }, { 1, 2, 1 }),
                 "ERROR: obj: Parameter 'xmax' must be larger than 'xmin'.");

    EXPECT_DEATH(g3dBoxMesh({ 1, 2, 0 }, { 2, 1, 1 }),
                 "ERROR: obj: Parameter 'ymax' must be larger than 'ymin'.");

    EXPECT_DEATH(g3dBoxMesh({ 0, 0, 2 }, { 1, 1, 1 }),
                 "ERROR: obj: Parameter 'zmax' must be larger than 'zmin'.");
}

TEST_F(BoxMeshTest, g3d_rectangle_mesh_create)
{
    auto obj = g3dBoxMesh({ 1, 2, 3 }, { 4, 5, 6 });
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 3);

    PetscReal gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 4);

    EXPECT_EQ(gmin[1], 2);
    EXPECT_EQ(gmax[1], 5);

    EXPECT_EQ(gmin[2], 3);
    EXPECT_EQ(gmax[2], 6);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    PetscInt n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 2160);
}
