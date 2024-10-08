#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LinearProblem_test.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/VTKOutput.h"
#include "petscdmda.h"
#include "petscviewer.h"

namespace {

class VTKOutputTest : public GodzillaAppTest {
protected:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        this->app->get_registry().add<G1DTestLinearProblem>("G1DTestLinearProblem");

        {
            Parameters * params = this->app->get_parameters("LineMesh");
            params->set<Int>("nx") = 1;
            this->mesh = this->app->build_object<LineMesh>("mesh", params);
        }

        {
            Parameters * params = this->app->get_parameters("G1DTestLinearProblem");
            params->set<MeshObject *>("_mesh_obj") = mesh;
            this->prob = this->app->build_object<G1DTestLinearProblem>("problem", params);
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
        Parameters * params = this->app->get_parameters("VTKOutput");
        params->set<Problem *>("_problem") = this->prob;
        if (file_name.length() > 0)
            params->set<std::string>("file") = file_name;
        VTKOutput * out = this->app->build_object<VTKOutput>("out", params);
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
    class MeshDA1D : public Mesh {
    public:
        MeshDA1D(mpi::Communicator comm) : Mesh()
        {
            DM dm;
            DMDACreate1d(comm, DM_BOUNDARY_NONE, 1, 1, 1, nullptr, &dm);
            set_dm(dm);
        }
    };

    class TestMesh : public MeshObject {
    public:
        explicit TestMesh(const Parameters & params) : MeshObject(params) {}

        Mesh *
        create_mesh() override
        {
            return new MeshDA1D(get_comm());
        }
    };

    class TestProblem : public LinearProblem {
    public:
        explicit TestProblem(const Parameters & params) : LinearProblem(params) {}
        void
        compute_rhs(Vector & b) override
        {
        }
        void compute_operators(Matrix & A, Matrix & B) override {};
    };

    testing::internal::CaptureStderr();

    Parameters mesh_pars = TestMesh::parameters();
    mesh_pars.set<App *>("_app") = this->app;
    mesh_pars.set<Int>("nx") = 1;
    TestMesh mesh(mesh_pars);

    Parameters prob_pars = TestProblem::parameters();
    prob_pars.set<App *>("_app") = this->app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    TestProblem prob(prob_pars);

    Parameters pars = VTKOutput::parameters();
    pars.set<App *>("_app") = this->app;
    pars.set<Problem *>("_problem") = &prob;
    VTKOutput out(pars);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("VTK output works only with unstructured meshes."));
}

TEST_F(VTKOutputTest, output_1d_step)
{
    auto out = build_output("out");
    create();

    this->app->check_integrity();

    this->prob->run();
    EXPECT_EQ(this->prob->converged(), true);
    out->output_step();
}
