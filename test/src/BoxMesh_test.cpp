#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/Parameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(BoxMeshTest, api)
{
    TestApp app;

    Parameters params = BoxMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "box_mesh";
    params.set<Real>("xmin") = 1;
    params.set<Real>("xmax") = 4;
    params.set<Int>("nx") = 9;
    params.set<Real>("ymin") = 2;
    params.set<Real>("ymax") = 5;
    params.set<Int>("ny") = 8;
    params.set<Real>("zmin") = 3;
    params.set<Real>("zmax") = 6;
    params.set<Int>("nz") = 7;
    BoxMesh mesh(params);

    EXPECT_EQ(mesh.get_x_min(), 1);
    EXPECT_EQ(mesh.get_x_max(), 4);
    EXPECT_EQ(mesh.get_nx(), 9);

    EXPECT_EQ(mesh.get_y_min(), 2);
    EXPECT_EQ(mesh.get_y_max(), 5);
    EXPECT_EQ(mesh.get_ny(), 8);

    EXPECT_EQ(mesh.get_z_min(), 3);
    EXPECT_EQ(mesh.get_z_max(), 6);
    EXPECT_EQ(mesh.get_nz(), 7);

    mesh.create();
    auto dm = mesh.get_dm();

    EXPECT_EQ(mesh.get_dimension(), 3);

    Real gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 4);

    EXPECT_EQ(gmin[1], 2);
    EXPECT_EQ(gmax[1], 5);

    EXPECT_EQ(gmin[2], 3);
    EXPECT_EQ(gmax[2], 6);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    Int n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 2160);
}

TEST(BoxMeshTest, incorrect_dims)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters params = BoxMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Real>("xmin") = 4;
    params.set<Real>("xmax") = 1;
    params.set<Int>("nx") = 9;
    params.set<Real>("ymin") = 5;
    params.set<Real>("ymax") = 2;
    params.set<Int>("ny") = 8;
    params.set<Real>("zmin") = 6;
    params.set<Real>("zmax") = 3;
    params.set<Int>("nz") = 7;
    BoxMesh mesh(params);

    app.check_integrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'zmax' must be larger than 'zmin'."));
}
