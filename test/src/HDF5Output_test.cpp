#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LinearProblem_test.h"
#include "LineMesh.h"
#include "Problem.h"
#include "HDF5Output.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

namespace {

class HDF5OutputTest : public GodzillaAppTest {
protected:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            Parameters * params = Factory::get_parameters(class_name);
            params->set<Int>("nx") = 1;
            this->mesh = this->app->build_object<LineMesh>(class_name, "mesh", params);
        }

        {
            const std::string class_name = "G1DTestLinearProblem";
            Parameters * params = Factory::get_parameters(class_name);
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

    HDF5Output *
    build_output(const std::string & file_name = "")
    {
        const std::string class_name = "HDF5Output";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Problem *>("_problem") = this->prob;
        if (file_name.length() > 0)
            params->set<std::string>("file") = file_name;
        HDF5Output * out = this->app->build_object<HDF5Output>(class_name, "out", params);
        this->prob->add_output(out);
        return out;
    }

    LineMesh * mesh;
    Problem * prob;
};

} // namespace

TEST_F(HDF5OutputTest, get_file_ext)
{
    auto out = build_output("out");
    EXPECT_EQ(out->get_file_ext(), "h5");
}

TEST_F(HDF5OutputTest, wrong_mesh_type)
{
    class TestMesh : public Mesh {
    public:
        explicit TestMesh(const Parameters & params) : Mesh(params) {}

        DM
        get_dm() const override
        {
            return this->dm;
        }

        void
        create() override
        {
            DMDACreate1d(get_comm(), DM_BOUNDARY_NONE, 1, 1, 1, nullptr, &this->dm);
            DMSetUp(this->dm);
        }

    protected:
        virtual void
        distribute() override
        {
        }

        DM dm;
    };

    class TestProblem : public LinearProblem {
    public:
        explicit TestProblem(const Parameters & params) : LinearProblem(params) {}

    protected:
        virtual PetscErrorCode
        compute_rhs_callback(Vec b) override
        {
            return 0;
        }
        virtual PetscErrorCode
        compute_operators_callback(Mat A, Mat B) override
        {
            return 0;
        }
    };

    testing::internal::CaptureStderr();

    Parameters mesh_pars = TestMesh::parameters();
    mesh_pars.set<const App *>("_app") = this->app;
    mesh_pars.set<Int>("nx") = 1;
    TestMesh mesh(mesh_pars);

    Parameters prob_pars = TestProblem::parameters();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    TestProblem prob(prob_pars);

    Parameters pars = HDF5Output::parameters();
    pars.set<const App *>("_app") = this->app;
    pars.set<const Problem *>("_problem") = &prob;
    HDF5Output out(pars);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    out.check();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("HDF5 output works only with unstructured meshes."));
}

TEST_F(HDF5OutputTest, set_file_name)
{
    auto out = build_output("out");
    create();
    out->set_file_name();
    EXPECT_EQ(out->get_file_name(), "out.h5");
}

TEST_F(HDF5OutputTest, set_seq_file_name)
{
    auto out = build_output("out");
    create();
    out->set_sequence_file_name(2);
    EXPECT_EQ(out->get_file_name(), "out.2.h5");
}

TEST_F(HDF5OutputTest, output)
{
    auto out = build_output("out");
    create();
    out->set_file_name();
    out->output_step();

    const std::string file_name = out->get_file_name();
    PetscViewer viewer;
    Vec sln;
    Real diff;
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
