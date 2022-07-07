#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "RectangleMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(RectangleMeshTest, api)
{
    TestApp app;

    Parameters params = RectangleMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "rect_mesh";
    params.set<PetscReal>("xmin") = 1;
    params.set<PetscReal>("xmax") = 3;
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = 2;
    params.set<PetscReal>("ymax") = 4;
    params.set<PetscInt>("ny") = 8;
    RectangleMesh mesh(params);

    EXPECT_EQ(mesh.get_x_min(), 1);
    EXPECT_EQ(mesh.get_x_max(), 3);
    EXPECT_EQ(mesh.get_nx(), 9);

    EXPECT_EQ(mesh.get_y_min(), 2);
    EXPECT_EQ(mesh.get_y_max(), 4);
    EXPECT_EQ(mesh.get_ny(), 8);

    mesh.create();
    DM dm = mesh.get_dm();

    EXPECT_EQ(mesh.get_dimension(), 2);

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

    Parameters params = RectangleMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<PetscReal>("xmin") = 2;
    params.set<PetscReal>("xmax") = 1;
    params.set<PetscInt>("nx") = 9;
    params.set<PetscReal>("ymin") = 2;
    params.set<PetscReal>("ymax") = 1;
    params.set<PetscInt>("ny") = 8;
    RectangleMesh mesh(params);

    app.check_integrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
}
