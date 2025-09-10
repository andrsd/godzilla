#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/Mesh.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(BoxMeshTest, api)
{
    TestApp app;

    auto params = BoxMesh::parameters();
    params.set<App *>("_app", &app)
        .set<std::string>("_name", "box_mesh")
        .set<Real>("xmin", 1)
        .set<Real>("xmax", 4)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 2)
        .set<Real>("ymax", 5)
        .set<Int>("ny", 8)
        .set<Real>("zmin", 3)
        .set<Real>("zmax", 6)
        .set<Int>("nz", 7);
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

    auto m = mesh.get_mesh<Mesh>();
    EXPECT_EQ(m->get_dimension(), 3_D);

    auto dm = m->get_dm();
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

    auto params = BoxMesh::parameters();
    params.set<App *>("_app", &app)
        .set<std::string>("_name", "obj")
        .set<Real>("xmin", 4)
        .set<Real>("xmax", 1)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 5)
        .set<Real>("ymax", 2)
        .set<Int>("ny", 8)
        .set<Real>("zmin", 6)
        .set<Real>("zmax", 3)
        .set<Int>("nz", 7);
    BoxMesh mesh(params);

    app.check_integrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'zmax' must be larger than 'zmin'."));
}
