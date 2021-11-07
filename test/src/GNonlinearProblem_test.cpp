#include "Factory.h"
#include "GGrid.h"
#include "GNonlinearProblem_test.h"
#include "InputParameters.h"
#include "petsc.h"
#include "petscvec.h"
#include "petscmat.h"


using namespace godzilla;

registerObject(GTestPetscNonlinearProblem);

TEST(GNonlinearProblemTest, solve)
{
    App app("test", MPI_COMM_WORLD);

    GGrid * grid;
    {
        const std::string class_name = "G1DStructuredGrid";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<PetscInt>("nx") = 2;
        grid = Factory::create<GGrid>(class_name, "grid", params);
    }

    const std::string class_name = "GTestPetscNonlinearProblem";
    InputParameters params = Factory::getValidParams(class_name);
    params.set<const App *>("_app") = &app;
    params.set<GGrid *>("_ggrid") = grid;
    auto prob = Factory::create<GTestPetscNonlinearProblem>(class_name, "obj", params);

    grid->create();
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // extract the solution and make sure it is [2, 3]
    const Vec x = prob->getSolutionVector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}



GTestPetscNonlinearProblem::GTestPetscNonlinearProblem(const InputParameters & params) :
    GNonlinearProblem(params)
{
}

GTestPetscNonlinearProblem::~GTestPetscNonlinearProblem()
{
}

PetscErrorCode
GTestPetscNonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    PetscInt ni = 2;
    PetscInt ix[] = {0, 1};
    PetscScalar y[2];
    VecGetValues(x, ni, ix, y);

    VecSetValue(f, 0, y[0] - 2, INSERT_VALUES);
    VecSetValue(f, 1, y[1] - 3, INSERT_VALUES);

    VecAssemblyBegin(f);
    VecAssemblyEnd(f);

    return 0;
}

PetscErrorCode
GTestPetscNonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    MatSetValue(J, 0, 0, 1, INSERT_VALUES);
    MatSetValue(J, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(J, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(J, MAT_FINAL_ASSEMBLY);

    return 0;
}
