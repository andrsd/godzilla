#include "base/GodzillaApp_test.h"
#include "grids/G2DRectangleMesh_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"

registerMooseObject("GodzillaApp", MockG2DRectangleMesh);

std::shared_ptr<MockG2DRectangleMesh>
g2dRectangleMesh(
        Factory & factory,
        const std::vector<PetscReal> & xmin,
        const std::vector<PetscReal> & xmax)
{
    const std::string class_name = "MockG2DRectangleMesh";
    InputParameters params = factory.getValidParams(class_name);
    params.set<PetscReal>("xmin") = xmin[0];
    params.set<PetscReal>("xmax") = xmax[0];
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = xmin[1];
    params.set<PetscReal>("ymax") = xmax[1];
    params.set<PetscInt>("ny") = 8;
    return factory.create<MockG2DRectangleMesh>(class_name, "obj", params);
}


TEST_F(GodzillaAppTest, g2d_rectangle_mesh)
{
    auto obj = g2dRectangleMesh(factory(), {1, 2}, {3, 4});

    EXPECT_EQ(obj->getXMin(), 1);
    EXPECT_EQ(obj->getXMax(), 3);
    EXPECT_EQ(obj->getNx(), 9);

    EXPECT_EQ(obj->getYMin(), 2);
    EXPECT_EQ(obj->getYMax(), 4);
    EXPECT_EQ(obj->getNy(), 8);
}

TEST_F(GodzillaAppTest, g2d_rectangle_mesh_incorrect_dims)
{
    EXPECT_DEATH(
        g2dRectangleMesh(factory(), {2, 1}, {1, 2}),
        "ERROR: obj: Parameter 'xmax' must be larger than 'xmin'."
    );

    EXPECT_DEATH(
        g2dRectangleMesh(factory(), {1, 2}, {2, 1}),
        "ERROR: obj: Parameter 'ymax' must be larger than 'ymin'."
    );
}

TEST_F(GodzillaAppTest, g2d_rectangle_mesh_create)
{
    auto obj = g2dRectangleMesh(factory(), {1, 2}, {3, 4});
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
