#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "RectangleMesh.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(RectangleMeshTest, api)
{
    TestApp app;

    InputParameters params = RectangleMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<PetscReal>("xmin") = 1;
    params.set<PetscReal>("xmax") = 3;
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = 2;
    params.set<PetscReal>("ymax") = 4;
    params.set<PetscInt>("ny") = 8;
    RectangleMesh mesh(params);

    EXPECT_EQ(mesh.getXMin(), 1);
    EXPECT_EQ(mesh.getXMax(), 3);
    EXPECT_EQ(mesh.getNx(), 9);

    EXPECT_EQ(mesh.getYMin(), 2);
    EXPECT_EQ(mesh.getYMax(), 4);
    EXPECT_EQ(mesh.getNy(), 8);

    mesh.create();
    DM dm = mesh.getDM();

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

TEST(RectangleMeshTest, incorrect_dims)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters params = RectangleMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<PetscReal>("xmin") = 2;
    params.set<PetscReal>("xmax") = 1;
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = 2;
    params.set<PetscReal>("ymax") = 1;
    params.set<PetscInt>("ny") = 8;
    RectangleMesh mesh(params);

    app.checkIntegrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
}
