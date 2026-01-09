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
    params.set<App *>("app", &app);
    params.set<String>("name", "line_mesh");
    params.set<Real>("xmin", 1);
    params.set<Real>("xmax", 2);
    params.set<Int>("nx", 10);
    auto mesh = MeshFactory::create<LineMesh>(params);

    auto dm = mesh->get_dm();

    EXPECT_EQ(mesh->get_dimension(), 1);

    auto bbox = mesh->get_bounding_box<1_D>();
    EXPECT_EQ(bbox.min()[0], 1);
    EXPECT_EQ(bbox.max()[0], 2);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    Int nx;
    VecGetSize(coords, &nx);
    EXPECT_EQ(nx, 11);
}

TEST(LineMeshTest, incorrect_dims)
{
    TestApp app;

    auto params = LineMesh::parameters();
    params.set<App *>("app", &app);
    params.set<String>("name", "line_mesh");
    params.set<Real>("xmin", 2);
    params.set<Real>("xmax", 1);
    params.set<Int>("nx", 2);

    EXPECT_DEATH(LineMesh mesh(params), "Parameter 'xmax' must be larger than 'xmin'.");
}
