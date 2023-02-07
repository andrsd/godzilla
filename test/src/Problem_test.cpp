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
        explicit TestProblem(const Parameters & params) : Problem(params) {}

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

        virtual const Vector &
        get_solution_vector() const
        {
            return this->sln;
        }

        Vector sln;
    };

    class TestPostprocessor : public Postprocessor {
    public:
        explicit TestPostprocessor(const Parameters & params) : Postprocessor(params) {}

        virtual void
        compute()
        {
        }

        virtual Real
        get_value()
        {
            return 0;
        }

        MOCK_METHOD(void, check, ());
    };

    class TestFunction : public Function {
    public:
        explicit TestFunction(const Parameters & params) : Function(params) {}

        virtual void
        register_callback(mu::Parser & parser)
        {
        }

        MOCK_METHOD(void, check, ());
    };

    class TestOutput : public Output {
    public:
        explicit TestOutput(const Parameters & params) : Output(params) {}

        MOCK_METHOD(void, output_step, ());
    };

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = Problem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    TestProblem problem(prob_params);

    Parameters pp_params = Postprocessor::parameters();
    pp_params.set<const App *>("_app") = &app;
    pp_params.set<const Problem *>("_problem") = &problem;
    pp_params.set<std::string>("_name") = "pp";
    TestPostprocessor pp(pp_params);
    problem.add_postprocessor(&pp);

    Parameters fn_params = Function::parameters();
    fn_params.set<const App *>("_app") = &app;
    fn_params.set<const Problem *>("_problem") = &problem;
    fn_params.set<std::string>("_name") = "fn";
    TestFunction fn(fn_params);
    problem.add_function(&fn);

    Parameters out_params = Function::parameters();
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
