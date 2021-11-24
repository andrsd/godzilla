#include "gtest/gtest.h"
#include "Godzilla.h"
#include "App.h"
#include "LineMesh.h"
#include "PoissonFENonlinearProblem.h"
#include "DirichletBC.h"

using namespace godzilla;

static PetscErrorCode
x2_fn(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar * u, void * ctx)
{
    u[0] = x[0] * x[0];
    return 0;
}

TEST(PoissonFENonlinearProblem, solve)
{
    App app("poisson-test", PETSC_COMM_WORLD);

    LineMesh * grid = nullptr;
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<PetscReal>("xmin") = 0.;
        params.set<PetscReal>("xmax") = 2.;
        params.set<PetscInt>("nx") = 20;
        grid = Factory::create<LineMesh>(class_name, "grid", params);
    }

    PoissonFENonlinearProblem * problem = nullptr;
    {
        const std::string class_name = "PoissonFENonlinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<Grid *>("_grid") = grid;
        params.set<PetscInt>("p_order") = 2;
        params.set<PetscReal>("forcing_fn") = -2;
        problem = Factory::create<PoissonFENonlinearProblem>(class_name, "problem", params);
    }

    DirichletBC * bc = nullptr;
    {
        const std::string class_name = "DirichletBC";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<std::vector<std::string>>("boundary") = { "marker" };
        params.set<std::vector<std::string>>("value") = { "x*x" };
        bc = Factory::create<DirichletBC>(class_name, "dirichlet", params);
        problem->addBoundaryCondition(bc);
    }

    grid->create();
    problem->create();

    problem->run();

    Vec u = problem->getSolutionVector();
    PetscReal error = 100.0;
    PetscFunc * exact_funcs[1] = { x2_fn };
    DMComputeL2Diff(grid->getDM(), 0.0, exact_funcs, NULL, u, &error);
    EXPECT_NEAR(error, 0, 1e-14);
}
