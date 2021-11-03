#include "base/GodzillaApp_test.h"
#include "grids/G1DLineMesh_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"

registerMooseObject("GodzillaApp", MockG1DLineMesh);


TEST_F(G1DLineMeshTest, g1d_line_mesh)
{
    auto o = g1dLineMesh(1, 2);

    EXPECT_EQ(o->getXMin(), 1);
    EXPECT_EQ(o->getXMax(), 2);
    EXPECT_EQ(o->getNx(), 10);
}

TEST_F(G1DLineMeshTest, g1d_line_mesh_incorrect_dims)
{
    EXPECT_DEATH(
        g1dLineMesh(2, 1),
        "ERROR: obj: Parameter 'xmax' must be larger than 'xmin'."
    );
}

TEST_F(G1DLineMeshTest, g1d_line_mesh_create)
{
    auto obj = g1dLineMesh(1, 2);
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 1);

    PetscReal gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 2);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    PetscInt nx;
    VecGetSize(coords, &nx);
    EXPECT_EQ(nx, 11);
}
