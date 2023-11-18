#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LinearProblem_test.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/VTKOutput.h"
#include "petsc.h"
#include "petscviewer.h"

namespace {

class VTKOutputTest : public GodzillaAppTest {
protected:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            Parameters * params = this->app->get_parameters(class_name);
            params->set<Int>("nx") = 1;
            this->mesh = this->app->build_object<LineMesh>(class_name, "mesh", params);
        }

        {
            const std::string class_name = "G1DTestLinearProblem";
            Parameters * params = this->app->get_parameters(class_name);
            params->set<Mesh *>("_mesh") = mesh;
            this->prob =
                this->app->build_object<G1DTestLinearProblem>(class_name, "problem", params);
        }
    }

    void
    create()
    {
        this->mesh->create();
        this->prob->create();
    }

    VTKOutput *
    build_output(const std::string & file_name = "")
    {
        const std::string class_name = "VTKOutput";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Problem *>("_problem") = this->prob;
        if (file_name.length() > 0)
            params->set<std::string>("file") = file_name;
        VTKOutput * out = this->app->build_object<VTKOutput>(class_name, "out", params);
        this->prob->add_output(out);
        return out;
    }

    LineMesh * mesh;
    G1DTestLinearProblem * prob;
};

} // namespace

TEST_F(VTKOutputTest, get_file_ext)
{
    auto out = build_output("out");
    EXPECT_EQ(out->get_file_ext(), "vtk");
}

TEST_F(VTKOutputTest, wrong_mesh_type)
{
    class TestMesh : public Mesh {
    public:
        explicit TestMesh(const Parameters & params) : Mesh(params) {}

        void
        create() override
        {
            DM dm;
            DMDACreate1d(get_comm(), DM_BOUNDARY_NONE, 1, 1, 1, nullptr, &dm);
            set_dm(dm);
            set_up();
        }

    protected:
        void
        distribute() override
        {
        }
    };

    class TestProblem : public LinearProblem {
    public:
        explicit TestProblem(const Parameters & params) : LinearProblem(params) {}

    protected:
        PetscErrorCode
        compute_rhs(Vector & b) override
        {
            return 0;
        }

        PetscErrorCode
        compute_operators(Matrix & A, Matrix & B) override
        {
            return 0;
        }
    };

    testing::internal::CaptureStderr();

    Parameters mesh_pars = TestMesh::parameters();
    mesh_pars.set<App *>("_app") = this->app;
    mesh_pars.set<Int>("nx") = 1;
    TestMesh mesh(mesh_pars);

    Parameters prob_pars = TestProblem::parameters();
    prob_pars.set<App *>("_app") = this->app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    TestProblem prob(prob_pars);

    Parameters pars = VTKOutput::parameters();
    pars.set<App *>("_app") = this->app;
    pars.set<Problem *>("_problem") = &prob;
    VTKOutput out(pars);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    out.check();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("VTK output works only with unstructured meshes."));
}

TEST_F(VTKOutputTest, output_1d_step)
{
    auto out = build_output("out");
    create();

    out->check();
    this->app->check_integrity();

    this->prob->solve();
    EXPECT_EQ(this->prob->converged(), true);
    out->output_step();
}
