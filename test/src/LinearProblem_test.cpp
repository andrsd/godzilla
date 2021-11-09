#include "Factory.h"
#include "GGrid.h"
#include "LinearProblem_test.h"
#include "InputParameters.h"
#include "petsc.h"
#include "petscmat.h"


using namespace godzilla;

registerObject(GTestPetscLinearProblem);

TEST(LinearProblemTest, solve)
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

    const std::string class_name = "GTestPetscLinearProblem";
    InputParameters params = Factory::getValidParams(class_name);
    params.set<const App *>("_app") = &app;
    params.set<GGrid *>("_ggrid") = grid;
    auto prob = Factory::create<GTestPetscLinearProblem>(class_name, "obj", params);

    grid->create();
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // extract the soluiton and make sure it is [2, 3]
    const Vec x = prob->getSolutionVector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}



GTestPetscLinearProblem::GTestPetscLinearProblem(const InputParameters & params) :
    LinearProblem(params)
{
}

GTestPetscLinearProblem::~GTestPetscLinearProblem()
{
}

PetscErrorCode
GTestPetscLinearProblem::computeRhsCallback(Vec b)
{
    VecSetValue(b, 0, 2, INSERT_VALUES);
    VecSetValue(b, 1, 3, INSERT_VALUES);

    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    return 0;
}

PetscErrorCode
GTestPetscLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    MatSetValue(A, 0, 0, 1, INSERT_VALUES);
    MatSetValue(A, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}
