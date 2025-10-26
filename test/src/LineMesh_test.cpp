#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Parameters.h"
#include "godzilla/LineMesh.h"

using namespace godzilla;

TEST(LineMeshTest, api)
{
    TestApp app;

    auto params = LineMesh::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("name", "line_mesh");
    params.set<Real>("xmin", 1);
    params.set<Real>("xmax", 2);
    params.set<Int>("nx", 10);
    auto mesh = MeshFactory::create<LineMesh>(params);

    auto dm = mesh->get_dm();

    EXPECT_EQ(mesh->get_dimension(), 1);

    Real gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 2);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    Int nx;
    VecGetSize(coords, &nx);
    EXPECT_EQ(nx, 11);
}

TEST(LineMeshTest, incorrect_dims)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = LineMesh::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("name", "line_mesh");
    params.set<Real>("xmin", 2);
    params.set<Real>("xmax", 1);
    params.set<Int>("nx", 2);
    LineMesh mesh(params);

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("line_mesh: Parameter 'xmax' must be larger than 'xmin'."));
}
