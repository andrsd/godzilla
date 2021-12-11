#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "Problem.h"
#include "Postprocessor.h"

using namespace godzilla;

TEST(ProblemTest, add_pp)
{
    TestApp app;

    class TestProblem : public Problem {
    public:
        TestProblem(const InputParameters & params) : Problem(params) {}

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
        getDM() const
        {
            return nullptr;
        }

        virtual Vec
        getSolutionVector() const
        {
            return nullptr;
        }
    };

    class TestPostprocessor : public Postprocessor {
    public:
        TestPostprocessor(const InputParameters & params) : Postprocessor(params) {}

        virtual void
        compute()
        {
        }

        virtual PetscReal
        getValue()
        {
            return 0;
        }
    };

    InputParameters prob_params = Problem::validParams();
    prob_params.set<const App *>("_app") = &app;
    TestProblem problem(prob_params);

    InputParameters pp_params = Postprocessor::validParams();
    pp_params.set<const App *>("_app") = &app;
    pp_params.set<Problem *>("_problem") = &problem;
    pp_params.set<std::string>("_name") = "pp";
    TestPostprocessor pp(pp_params);

    problem.addPostprocessor(&pp);
    problem.check();

    EXPECT_EQ(problem.getPostprocessor("pp"), &pp);
    EXPECT_EQ(problem.getPostprocessor("asdf"), nullptr);
}
