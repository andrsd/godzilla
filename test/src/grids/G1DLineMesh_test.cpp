#include "base/GodzillaApp_test.h"
#include "grids/G1DLineMesh_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"

registerMooseObject("GodzillaApp", MockG1DLineMesh);

std::shared_ptr<MockG1DLineMesh>
g1dLineMesh(Factory & factory, PetscReal xmin, PetscReal xmax)
{
    const std::string class_name = "MockG1DLineMesh";
    InputParameters params = factory.getValidParams(class_name);
    params.set<PetscReal>("xmin") = xmin;
    params.set<PetscReal>("xmax") = xmax;
    params.set<PetscInt>("nx") = 10;
    return factory.create<MockG1DLineMesh>(class_name, "obj", params);
}


TEST_F(GodzillaAppTest, g1d_line_mesh)
{
    auto obj = g1dLineMesh(factory(), 1, 2);

    EXPECT_EQ(obj->getXMin(), 1);
    EXPECT_EQ(obj->getXMax(), 2);
    EXPECT_EQ(obj->getNx(), 10);
}

TEST_F(GodzillaAppTest, g1d_line_mesh_incorrect_dims)
{
    EXPECT_DEATH(
        g1dLineMesh(factory(), 2, 1),
        "ERROR: obj: Parameter 'xmax' must be larger than 'xmin'."
    );
}

TEST_F(GodzillaAppTest, g1d_line_mesh_create)
{
    auto obj = g1dLineMesh(factory(), 1, 2);
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
