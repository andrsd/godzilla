#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(RectangleMeshTest, api)
{
    TestApp app;

    auto params = RectangleMesh::parameters();
    params.set<App *>("_app", &app)
        .set<std::string>("_name", "rect_mesh")
        .set<Real>("xmin", 1)
        .set<Real>("xmax", 3)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 2)
        .set<Real>("ymax", 4)
        .set<Int>("ny", 8);
    RectangleMesh mesh(params);

    EXPECT_EQ(mesh.get_x_min(), 1);
    EXPECT_EQ(mesh.get_x_max(), 3);
    EXPECT_EQ(mesh.get_nx(), 9);

    EXPECT_EQ(mesh.get_y_min(), 2);
    EXPECT_EQ(mesh.get_y_max(), 4);
    EXPECT_EQ(mesh.get_ny(), 8);

    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto dm = m->get_dm();

    EXPECT_EQ(m->get_dimension(), 2);

    Real gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 3);

    EXPECT_EQ(gmin[1], 2);
    EXPECT_EQ(gmax[1], 4);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    Int n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 180);
}

TEST(RectangleMeshTest, incorrect_dims)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = RectangleMesh::parameters();
    params.set<App *>("_app", &app)
        .set<std::string>("_name", "obj")
        .set<Real>("xmin", 2)
        .set<Real>("xmax", 1)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 2)
        .set<Real>("ymax", 1)
        .set<Int>("ny", 8);
    RectangleMesh mesh(params);

    app.check_integrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
}
