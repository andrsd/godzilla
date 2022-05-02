#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "Problem.h"

using namespace godzilla;

TEST(ProblemTest, test)
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

    InputParameters prob_params = Problem::valid_params();
    prob_params.set<const App *>("_app") = &app;
    TestProblem problem(prob_params);

    problem.create();
    problem.check();

    EXPECT_EQ(problem.get_time(), 0.);
}
