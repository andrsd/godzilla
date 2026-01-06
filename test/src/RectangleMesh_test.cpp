#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

TEST(RectangleMeshTest, api)
{
    TestApp app;

    auto params = RectangleMesh::parameters();
    params.set<App *>("app", &app)
        .set<String>("name", "rect_mesh")
        .set<Real>("xmin", 1)
        .set<Real>("xmax", 3)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 2)
        .set<Real>("ymax", 4)
        .set<Int>("ny", 8);
    auto mesh = MeshFactory::create<RectangleMesh>(params);

    auto dm = mesh->get_dm();

    EXPECT_EQ(mesh->get_dimension(), 2);

    auto bbox = mesh->get_bounding_box<2_D>();
    EXPECT_EQ(bbox.min()[0], 1);
    EXPECT_EQ(bbox.max()[0], 3);

    EXPECT_EQ(bbox.min()[1], 2);
    EXPECT_EQ(bbox.max()[1], 4);

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
    params.set<App *>("app", &app)
        .set<String>("name", "obj")
        .set<Real>("xmin", 2)
        .set<Real>("xmax", 1)
        .set<Int>("nx", 9)
        .set<Real>("ymin", 2)
        .set<Real>("ymax", 1)
        .set<Int>("ny", 8);
    RectangleMesh mesh(params);

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'xmax' must be larger than 'xmin'."));
    EXPECT_THAT(output, testing::HasSubstr("obj: Parameter 'ymax' must be larger than 'ymin'."));
}
