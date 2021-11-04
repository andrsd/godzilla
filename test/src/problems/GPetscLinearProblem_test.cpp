#include "base/GodzillaApp_test.h"
#include "problems/GPetscLinearProblem_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"
#include "petscmat.h"

registerMooseObject("GodzillaApp", GTestPetscLinearProblem);


TEST_F(GPetscLinearProblemTest, solve)
{
    const std::string class_name = "GTestPetscLinearProblem";
    InputParameters params = factory().getValidParams(class_name);
    params.set<GGrid *>("_ggrid") = nullptr;
    auto prob = factory().create<GTestPetscLinearProblem>(class_name, "obj", params);
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // TODO: extract the soluiton and make sure it is [2, 3]
}



GTestPetscLinearProblem::GTestPetscLinearProblem(const InputParameters & params) :
    GPetscLinearProblem(params)
{
    DMDACreate1d(comm(), DM_BOUNDARY_NONE, 2, 1, 1, NULL, &this->dm);
    DMSetUp(this->dm);
}

GTestPetscLinearProblem::~GTestPetscLinearProblem()
{
    DMDestroy(&this->dm);
}

const DM &
GTestPetscLinearProblem::getDM()
{
    return this->dm;
}

void
GTestPetscLinearProblem::setupProblem()
{
}

void
GTestPetscLinearProblem::out()
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
