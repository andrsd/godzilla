#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "LinearProblem_test.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/LineMesh.h"

using namespace godzilla;

TEST(ExodusIIOutputTest, get_file_ext)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    EXPECT_EQ(out.get_file_ext(), "exo");
}

TEST(ExodusIIOutputTest, create)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    ExodusIIOutput out(params);

    prob.add_output(&out);
    out.create();
}

TEST(ExodusIIOutputTest, non_existent_var)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    params.set<std::vector<std::string>>("variables") = { "asdf" };
    ExodusIIOutput out(params);

    prob.add_output(&out);
    mesh.create();
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Variable 'asdf' specified in 'variables' parameter does not exist. Typo?"));
}

TEST(ExodusIIOutputTest, fe_check)
{
    class EmptyMesh : public Mesh {
    public:
        EmptyMesh(mpi::Communicator comm) : Mesh()
        {
            DM dm;
            DMCreate(comm, &dm);
            set_dm(dm);
        }
    };

    class TestMesh : public MeshObject {
    public:
        explicit TestMesh(const Parameters & params) : MeshObject(params) {}

        Mesh *
        create_mesh() override
        {
            return new EmptyMesh(get_comm());
        }
    };

    class TestLinearProblem : public Problem {
    public:
        explicit TestLinearProblem(const Parameters & params) : Problem(params) {}

        void
        run()
        {
        }
        void
        solve()
        {
        }
        bool
        converged()
        {
            return true;
        }
    };

    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = MeshObject::parameters();
    mesh_pars.set<App *>("_app") = &app;
    TestMesh mesh(mesh_pars);

    Parameters prob_pars = TestLinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    TestLinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    ExodusIIOutput out(params);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::AllOf(testing::HasSubstr(
                    "ExodusII output can be only used with unstructured meshes.")));
}

TEST(ExodusIIOutputTest, output)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    ExodusIIOutput out(params);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    app.check_integrity();

    out.output_step();
}

TEST(ExodusIIOutputTest, set_file_name)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    out.create();

    EXPECT_EQ(out.get_file_name(), "out.exo");
}

TEST(ExodusIIOutputTest, set_seq_file_name)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    out.create();

    out.set_sequence_file_base(2);
    EXPECT_EQ(out.get_file_name(), "out.2.exo");
}
