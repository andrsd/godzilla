#include "gmock/gmock.h"
#include "GTestFENonlinearProblem.h"
#include "VTKOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(VTKOutputTest, get_file_ext)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->get_file_ext(), "vtk");
}

TEST_F(VTKOutputTest, create)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    prob->add_output(out);
    out->create();
}

TEST_F(VTKOutputTest, check)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    out->check();
}

TEST_F(VTKOutputTest, wrong_mesh_type)
{
    class TestMesh : public Mesh {
    public:
        explicit TestMesh(const InputParameters & params) : Mesh(params) {}

    protected:
        virtual void
        create_dm()
        {
            DMDACreate1d(get_comm(), DM_BOUNDARY_NONE, 1, 1, 1, nullptr, &this->dm);
            DMSetUp(this->dm);
        }

        virtual void
        distribute()
        {
        }
    };

    class TestProblem : public LinearProblem {
    public:
        explicit TestProblem(const InputParameters & params) : LinearProblem(params) {}

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

    InputParameters mesh_pars = Mesh::valid_params();
    mesh_pars.set<const App *>("_app") = this->app;
    mesh_pars.set<PetscInt>("nx") = 1;
    TestMesh mesh(mesh_pars);

    InputParameters prob_pars = TestProblem::valid_params();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    TestProblem prob(prob_pars);

    InputParameters pars = VTKOutput::valid_params();
    pars.set<const App *>("_app") = this->app;
    pars.set<const Problem *>("_problem") = &prob;
    VTKOutput out(pars);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    out.check();
    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("VTK output works only with unstructured meshes."));
}

TEST_F(VTKOutputTest, output_1d_step)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->check_integrity();

    prob->solve();
    EXPECT_EQ(prob->converged(), true);
    out->output_step();
}
