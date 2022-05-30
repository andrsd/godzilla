#include "GodzillaApp_test.h"
#include "HDF5Output_test.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

TEST_F(HDF5OutputTest, get_file_ext)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->get_file_ext(), "h5");
}

TEST_F(HDF5OutputTest, create)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    prob->add_output(out);
    prob->create();
}

TEST_F(HDF5OutputTest, check)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    out->check();
}

TEST_F(HDF5OutputTest, set_file_name)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    out->create();
    out->set_file_name();
    EXPECT_EQ(out->get_file_name(), "out.h5");
}

TEST_F(HDF5OutputTest, set_seq_file_name)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    out->create();
    out->set_sequence_file_name(2);
    EXPECT_EQ(out->get_file_name(), "out.2.h5");
}

TEST_F(HDF5OutputTest, output)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->set_file_name();
    out->output_step();

    const std::string file_name = out->get_file_name();
    PetscViewer viewer;
    Vec sln;
    PetscReal diff;
    DMCreateGlobalVector(mesh->get_dm(), &sln);
    PetscObjectSetName((PetscObject) sln, "sln");
    PetscViewerHDF5Open(PETSC_COMM_WORLD, file_name.c_str(), FILE_MODE_READ, &viewer);
    VecLoad(sln, viewer);
    VecAXPY(sln, -1.0, prob->get_solution_vector());
    VecNorm(sln, NORM_INFINITY, &diff);
    EXPECT_LT(diff, PETSC_MACHINE_EPSILON);
    PetscViewerDestroy(&viewer);
    VecDestroy(&sln);
}

// TODO: write a test for output of sequence
