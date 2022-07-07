#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LinearProblem_test.h"
#include "LineMesh.h"
#include "Problem.h"
#include "MeshPartitioningOutput.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

class MeshPartitioningOutputTest : public GodzillaAppTest {
protected:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            Parameters * params = Factory::get_valid_params(class_name);
            params->set<PetscReal>("xmin") = 0;
            params->set<PetscReal>("xmax") = 1;
            params->set<PetscInt>("nx") = 4;
            this->mesh = this->app->build_object<LineMesh>(class_name, "mesh", params);
        }

        {
            const std::string class_name = "G1DTestLinearProblem";
            Parameters * params = Factory::get_valid_params(class_name);
            params->set<const Mesh *>("_mesh") = mesh;
            this->prob = this->app->build_object<Problem>(class_name, "problem", params);
        }
    }

    void
    create()
    {
        this->mesh->create();
        this->prob->create();
    }

    MeshPartitioningOutput *
    build_output()
    {
        const std::string class_name = "MeshPartitioningOutput";
        Parameters * params = Factory::get_valid_params(class_name);
        params->set<const Problem *>("_problem") = this->prob;
        MeshPartitioningOutput * out =
            this->app->build_object<MeshPartitioningOutput>(class_name, "out", params);
        this->prob->add_output(out);
        return out;
    }

    LineMesh * mesh;
    Problem * prob;
};

TEST_F(MeshPartitioningOutputTest, get_file_ext)
{
    auto out = build_output();
    EXPECT_EQ(out->get_file_ext(), "h5");
}

TEST_F(MeshPartitioningOutputTest, output)
{
    auto out = build_output();
    create();
    out->output_step();

    PetscViewer viewer;
    Vec p;
    VecCreate(this->app->get_comm(), &p);
    PetscObjectSetName((PetscObject) p, "fields/partitioning");
    PetscViewerHDF5Open(this->app->get_comm(), "part.h5", FILE_MODE_READ, &viewer);
    VecLoad(p, viewer);
    PetscReal l2_norm;
    VecNorm(p, NORM_2, &l2_norm);
    EXPECT_NEAR(l2_norm, 0, 1e-10);
    VecDestroy(&p);
    PetscViewerDestroy(&viewer);
}
