#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "LinearProblem_test.h"
#include "ExodusIIOutput.h"
#include "petsc.h"

using namespace godzilla;

namespace {

class ExodusIIOutputTest : public FENonlinearProblemTest {
};

} // namespace

TEST_F(ExodusIIOutputTest, get_file_ext)
{
    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    EXPECT_EQ(out.get_file_ext(), "exo");
}

TEST_F(ExodusIIOutputTest, create)
{
    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    ExodusIIOutput out(params);

    this->prob->add_output(&out);
    out.create();
}

TEST_F(ExodusIIOutputTest, non_existent_var)
{
    testing::internal::CaptureStderr();

    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::vector<std::string>>("variables") = { "asdf" };
    ExodusIIOutput out(params);

    this->prob->add_output(&out);
    this->mesh->create();
    this->prob->create();

    out.check();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Variable 'asdf' specified in 'variables' parameter does not exist. Typo?"));
}

TEST_F(ExodusIIOutputTest, check)
{
    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    ExodusIIOutput out(params);

    out.check();
}

TEST_F(ExodusIIOutputTest, fe_check)
{
    class TestMesh : public Mesh {
    public:
        explicit TestMesh(const Parameters & params) : Mesh(params) {}

    protected:
        virtual void
        create_dm()
        {
            DMCreate(get_comm(), &this->dm);
        }

        virtual void
        distribute()
        {
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
        DM
        get_dm() const
        {
            return nullptr;
        }
        Vec
        get_solution_vector() const
        {
            return nullptr;
        }
    };

    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = Mesh::parameters();
    mesh_pars.set<const App *>("_app") = this->app;
    TestMesh mesh(mesh_pars);

    Parameters prob_pars = TestLinearProblem::parameters();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    TestLinearProblem prob(prob_pars);

    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = &prob;
    ExodusIIOutput out(params);

    mesh.create();
    prob.create();

    out.check();
    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::AllOf(
            testing::HasSubstr("ExodusII output can be only used with unstructured meshes."),
            testing::HasSubstr("ExodusII output can be only used with finite element problems.")));
}

TEST_F(ExodusIIOutputTest, output)
{
    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    ExodusIIOutput out(params);

    this->mesh->create();
    this->prob->create();

    out.check();
    this->app->check_integrity();

    out.output_step();
}

TEST_F(ExodusIIOutputTest, set_file_name)
{
    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    out.create();

    out.set_file_name();
    EXPECT_EQ(out.get_file_name(), "out.exo");
}

TEST_F(ExodusIIOutputTest, set_seq_file_name)
{
    Parameters params = ExodusIIOutput::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    out.create();

    out.set_sequence_file_name(2);
    EXPECT_EQ(out.get_file_name(), "out.2.exo");
}
