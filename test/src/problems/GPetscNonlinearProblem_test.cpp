#include "base/Factory.h"
#include "base/GodzillaApp_test.h"
#include "problems/GPetscNonlinearProblem_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"
#include "petscvec.h"
#include "petscmat.h"


using namespace godzilla;

registerObject(GTestPetscNonlinearProblem);

TEST(GPetscNonlinearProblemTest, solve)
{
    App app("test", MPI_COMM_WORLD);

    const std::string class_name = "GTestPetscNonlinearProblem";
    InputParameters params = Factory::getValidParams(class_name);
    params.set<const App *>("_app") = &app;
    params.set<GGrid *>("_ggrid") = nullptr;
    auto prob = Factory::create<GTestPetscNonlinearProblem>(class_name, "obj", params);
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // TODO: extract the soluiton and make sure it is [2, 3]
    delete prob;
}



GTestPetscNonlinearProblem::GTestPetscNonlinearProblem(const InputParameters & params) :
    GPetscNonlinearProblem(params)
{
    DMDACreate1d(comm(), DM_BOUNDARY_NONE, 2, 1, 1, NULL, &this->dm);
    DMSetUp(this->dm);
}

GTestPetscNonlinearProblem::~GTestPetscNonlinearProblem()
{
    DMDestroy(&this->dm);
}

const DM &
GTestPetscNonlinearProblem::getDM()
{
    return this->dm;
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
