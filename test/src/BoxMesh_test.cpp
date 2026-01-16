#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(BoxMeshTest, api)
{
    TestApp app;

    auto params = BoxMesh::parameters();
    params.set<App *>("app", &app)
        .set<String>("name", "box_mesh")
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

    auto m = mesh.create_mesh();

    EXPECT_EQ(m->get_dimension(), 3_D);

    auto bbox = m->get_bounding_box<3_D>();
    EXPECT_EQ(bbox.min()[0], 1);
    EXPECT_EQ(bbox.max()[0], 4);

    EXPECT_EQ(bbox.min()[1], 2);
    EXPECT_EQ(bbox.max()[1], 5);

    EXPECT_EQ(bbox.min()[2], 3);
    EXPECT_EQ(bbox.max()[2], 6);

    auto dm = m->get_dm();
    Vec coords;
    DMGetCoordinates(dm, &coords);
    Int n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 2160);
}

TEST(BoxMeshTest, incorrect_dims)
{
    TestApp app;

    auto params = BoxMesh::parameters();
    params.set<App *>("app", &app)
        .set<String>("name", "obj")
        .set<Real>("xmin", 4)
        .set<Real>("xmax", 1)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 5)
        .set<Real>("ymax", 2)
        .set<Int>("ny", 8)
        .set<Real>("zmin", 6)
        .set<Real>("zmax", 3)
        .set<Int>("nz", 7);

    EXPECT_DEATH(BoxMesh mesh(params), "Parameter 'xmax' must be larger than 'xmin'.");
}
