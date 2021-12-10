#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "InputParameters.h"
#include "LineMesh.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

using namespace godzilla;

TEST(LineMeshTest, api)
{
    TestApp app;

    InputParameters params = LineMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<PetscReal>("xmin") = 1;
    params.set<PetscReal>("xmax") = 2;
    params.set<PetscInt>("nx") = 10;
    LineMesh mesh(params);

    EXPECT_EQ(mesh.getXMin(), 1);
    EXPECT_EQ(mesh.getXMax(), 2);
    EXPECT_EQ(mesh.getNx(), 10);

    mesh.create();
    DM dm = mesh.getDM();

    EXPECT_EQ(mesh.getDimension(), 1);

    PetscReal gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 1);
    EXPECT_EQ(gmax[0], 2);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    PetscInt nx;
    VecGetSize(coords, &nx);
    EXPECT_EQ(nx, 11);
}

TEST(LineMeshTest, incorrect_dims)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters params = LineMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<PetscReal>("xmin") = 2;
    params.set<PetscReal>("xmax") = 1;
    params.set<PetscInt>("nx") = 2;
    LineMesh mesh(params);

    app.checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("line_mesh: Parameter 'xmax' must be larger than 'xmin'."));
}

TEST(LineMeshTest, distribute)
{
    PetscMPIInt sz;
    MPI_Comm_size(PETSC_COMM_WORLD, &sz);

    TestApp app;

    InputParameters params = LineMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<PetscReal>("xmin") = 0;
    params.set<PetscReal>("xmax") = 1;
    params.set<PetscInt>("nx") = 4;
    LineMesh mesh(params);
    mesh.create();

    PetscBool distr;
    DMPlexIsDistributed(mesh.getDM(), &distr);
    if (sz > 1)
        EXPECT_EQ(distr, 1);
    else
        EXPECT_EQ(distr, 0);
}

TEST(LineMeshTest, output_partitioning)
{
    TestApp app;

    InputParameters params = LineMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "line_mesh";
    params.set<PetscReal>("xmin") = 0;
    params.set<PetscReal>("xmax") = 1;
    params.set<PetscInt>("nx") = 4;
    LineMesh mesh(params);
    mesh.create();

    PetscViewer viewer;
    PetscViewerHDF5Open(app.getComm(), "part.h5", FILE_MODE_WRITE, &viewer);
    mesh.outputPartitioning(viewer);
    PetscViewerDestroy(&viewer);

    Vec p;
    VecCreate(app.getComm(), &p);
    PetscObjectSetName((PetscObject) p, "fields/partitioning");
    PetscViewerHDF5Open(app.getComm(), "part.h5", FILE_MODE_READ, &viewer);
    VecLoad(p, viewer);
    PetscReal l2_norm;
    VecNorm(p, NORM_2, &l2_norm);
    EXPECT_NEAR(l2_norm, 0, 1e-10);
    VecDestroy(&p);
    PetscViewerDestroy(&viewer);
}
