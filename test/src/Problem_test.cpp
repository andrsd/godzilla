#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "LineMesh.h"
#include "Problem.h"
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
    pp_params.set<Problem *>("_problem") = &problem;
    pp_params.set<std::string>("_name") = "pp";
    TestPostprocessor pp(pp_params);

    problem.add_postprocessor(&pp);
    problem.check();

    EXPECT_EQ(problem.get_postprocessor("pp"), &pp);
    EXPECT_EQ(problem.get_postprocessor("asdf"), nullptr);
}
