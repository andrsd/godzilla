#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LinearProblem_test.h"
#include "LineMesh.h"
#include "Problem.h"
#include "VTKOutput.h"
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

    VTKOutput *
    build_output(const std::string & file_name = "")
    {
        const std::string class_name = "VTKOutput";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Problem *>("_problem") = this->prob;
        if (file_name.length() > 0)
            params->set<std::string>("file") = file_name;
        VTKOutput * out = this->app->build_object<VTKOutput>(class_name, "out", params);
        this->prob->add_output(out);
        return out;
    }

    LineMesh * mesh;
    Problem * prob;
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

    Parameters pars = VTKOutput::parameters();
    pars.set<const App *>("_app") = this->app;
    pars.set<const Problem *>("_problem") = &prob;
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
