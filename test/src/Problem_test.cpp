#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LineMesh.h"
#include "Problem.h"
#include "Function.h"
#include "Output.h"
#include "Postprocessor.h"

using namespace godzilla;

TEST(ProblemTest, add_pp)
{
    TestApp app;

    class TestProblem : public Problem {
    public:
        explicit TestProblem(const InputParameters & params) : Problem(params) {}

        virtual void
        run()
        {
        }
        virtual void
        solve()
        {
        }
        virtual bool
        converged()
        {
            return false;
        }
        virtual DM
        get_dm() const
        {
            return nullptr;
        }

        virtual Vec
        get_solution_vector() const
        {
            return nullptr;
        }
    };

    class TestPostprocessor : public Postprocessor {
    public:
        explicit TestPostprocessor(const InputParameters & params) : Postprocessor(params) {}

        virtual void
        compute()
        {
        }

        virtual PetscReal
        get_value()
        {
            return 0;
        }

        MOCK_METHOD(void, check, ());
    };

    class TestFunction : public Function {
    public:
        explicit TestFunction(const InputParameters & params) : Function(params) {}

        virtual void
        register_callback(mu::Parser & parser)
        {
        }

        MOCK_METHOD(void, check, ());
    };

    class TestOutput : public Output {
    public:
        explicit TestOutput(const InputParameters & params) : Output(params) {}

        MOCK_METHOD(void, output_step, ());
    };

    InputParameters mesh_params = LineMesh::valid_params();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    InputParameters prob_params = Problem::valid_params();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);

    InputParameters pp_params = Postprocessor::valid_params();
    pp_params.set<const App *>("_app") = &app;
    pp_params.set<const Problem *>("_problem") = &problem;
    pp_params.set<std::string>("_name") = "pp";
    TestPostprocessor pp(pp_params);
    problem.add_postprocessor(&pp);

    InputParameters fn_params = Function::valid_params();
    fn_params.set<const App *>("_app") = &app;
    fn_params.set<const Problem *>("_problem") = &problem;
    fn_params.set<std::string>("_name") = "fn";
    TestFunction fn(fn_params);
    problem.add_function(&fn);

    InputParameters out_params = Function::valid_params();
    out_params.set<const App *>("_app") = &app;
    out_params.set<const Problem *>("_problem") = &problem;
    out_params.set<std::string>("_name") = "out";
    out_params.set<std::vector<std::string>>("on") = { "initial" };
    TestOutput out(out_params);
    out.create();
    problem.add_output(&out);

    EXPECT_CALL(fn, check);
    EXPECT_CALL(pp, check);
    problem.check();

    EXPECT_EQ(problem.get_postprocessor("pp"), &pp);
    EXPECT_EQ(problem.get_postprocessor("asdf"), nullptr);

    auto & pps_names = problem.get_postprocessor_names();
    EXPECT_EQ(pps_names.size(), 1);
    EXPECT_EQ(pps_names[0], "pp");

    EXPECT_CALL(out, output_step);
    problem.output(Output::ON_INITIAL);
}
