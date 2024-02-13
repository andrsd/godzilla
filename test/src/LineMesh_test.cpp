#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Parameters.h"
#include "godzilla/LineMesh.h"
#include "petsc.h"

using namespace godzilla;

TEST(LineMeshTest, api)
{
    TestApp app;

    Parameters params = LineMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<Real>("xmin") = 1;
    params.set<Real>("xmax") = 2;
    params.set<Int>("nx") = 10;
    LineMesh mesh(params);

    EXPECT_EQ(mesh.get_x_min(), 1);
    EXPECT_EQ(mesh.get_x_max(), 2);
    EXPECT_EQ(mesh.get_nx(), 10);

    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto dm = m->get_dm();

    EXPECT_EQ(m->get_dimension(), 1);

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

    Parameters params = LineMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<Real>("xmin") = 2;
    params.set<Real>("xmax") = 1;
    params.set<Int>("nx") = 2;
    LineMesh mesh(params);

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("line_mesh: Parameter 'xmax' must be larger than 'xmin'."));
}

TEST(LineMeshTest, distribute)
{
    PetscMPIInt sz;
    MPI_Comm_size(PETSC_COMM_WORLD, &sz);

    TestApp app;

    Parameters params = LineMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<Real>("xmin") = 0;
    params.set<Real>("xmax") = 1;
    params.set<Int>("nx") = 4;
    LineMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<Mesh>();
    PetscBool distr;
    DMPlexIsDistributed(m->get_dm(), &distr);
    if (sz > 1)
        EXPECT_EQ(distr, 1);
    else
        EXPECT_EQ(distr, 0);
}
