#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "BoxMesh.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(BoxMeshTest, api)
{
    TestApp app;

    InputParameters params = BoxMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "box_mesh";
    params.set<PetscReal>("xmin") = 1;
    params.set<PetscReal>("xmax") = 4;
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = 2;
    params.set<PetscReal>("ymax") = 5;
    params.set<PetscInt>("ny") = 8;
    params.set<PetscReal>("zmin") = 3;
    params.set<PetscReal>("zmax") = 6;
    params.set<PetscInt>("nz") = 7;
    BoxMesh mesh(params);

    EXPECT_EQ(mesh.getXMin(), 1);
    EXPECT_EQ(mesh.getXMax(), 4);
    EXPECT_EQ(mesh.getNx(), 9);

    EXPECT_EQ(mesh.getYMin(), 2);
    EXPECT_EQ(mesh.getYMax(), 5);
    EXPECT_EQ(mesh.getNy(), 8);

    EXPECT_EQ(mesh.getZMin(), 3);
    EXPECT_EQ(mesh.getZMax(), 6);
    EXPECT_EQ(mesh.getNz(), 7);

    mesh.create();
    DM dm = mesh.getDM();

    EXPECT_EQ(mesh.getDimension(), 3);

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

TEST(BoxMeshTest, incorrect_dims)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters params = BoxMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<PetscReal>("xmin") = 4;
    params.set<PetscReal>("xmax") = 1;
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = 5;
    params.set<PetscReal>("ymax") = 2;
    params.set<PetscInt>("ny") = 8;
    params.set<PetscReal>("zmin") = 6;
    params.set<PetscReal>("zmax") = 3;
    params.set<PetscInt>("nz") = 7;
    BoxMesh mesh(params);

    app.checkIntegrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'zmax' must be larger than 'zmin'."));
}
