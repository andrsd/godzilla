#include "Factory.h"
#include "Grid.h"
#include "LinearProblem_test.h"
#include "InputParameters.h"
#include "petsc.h"
#include "petscmat.h"

using namespace godzilla;

registerObject(G1DTestPetscLinearProblem);
registerObject(G2DTestPetscLinearProblem);
registerObject(G3DTestPetscLinearProblem);

TEST(LinearProblemTest, solve)
{
    App app("test", MPI_COMM_WORLD);

    Grid * grid;
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        grid = app.buildObject<Grid>(class_name, "grid", params);
    }

    const std::string class_name = "G1DTestPetscLinearProblem";
    InputParameters & params = Factory::getValidParams(class_name);
    params.set<Grid *>("_grid") = grid;
    auto prob = app.buildObject<G1DTestPetscLinearProblem>(class_name, "obj", params);

    grid->create();
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // extract the solution and make sure it is [2, 3]
    const Vec & x = prob->getSolutionVector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}

// 1D

G1DTestPetscLinearProblem::G1DTestPetscLinearProblem(const InputParameters & params) :
    LinearProblem(params),
    s(nullptr)
{
}

G1DTestPetscLinearProblem::~G1DTestPetscLinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G1DTestPetscLinearProblem::create()
{
    const DM & dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[2] = { 1, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G1DTestPetscLinearProblem::computeRhsCallback(Vec b)
{
    VecSetValue(b, 0, 2, INSERT_VALUES);
    VecSetValue(b, 1, 3, INSERT_VALUES);

    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    return 0;
}

PetscErrorCode
G1DTestPetscLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    MatSetValue(A, 0, 0, 1, INSERT_VALUES);
    MatSetValue(A, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}

// 2D

G2DTestPetscLinearProblem::G2DTestPetscLinearProblem(const InputParameters & params) :
    LinearProblem(params),
    s(nullptr)
{
}

G2DTestPetscLinearProblem::~G2DTestPetscLinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G2DTestPetscLinearProblem::create()
{
    const DM & dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[3] = { 1, 0, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G2DTestPetscLinearProblem::computeRhsCallback(Vec b)
{
    VecSetValue(b, 0, 2, INSERT_VALUES);
    VecSetValue(b, 1, 3, INSERT_VALUES);
    VecSetValue(b, 2, 5, INSERT_VALUES);
    VecSetValue(b, 3, 8, INSERT_VALUES);

    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    return 0;
}

PetscErrorCode
G2DTestPetscLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    for (PetscInt i = 0; i < 4; i++)
        MatSetValue(A, i, i, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}

// 3D

G3DTestPetscLinearProblem::G3DTestPetscLinearProblem(const InputParameters & params) :
    LinearProblem(params),
    s(nullptr)
{
}

G3DTestPetscLinearProblem::~G3DTestPetscLinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G3DTestPetscLinearProblem::create()
{
    const DM & dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[4] = { 1, 0, 0, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G3DTestPetscLinearProblem::computeRhsCallback(Vec b)
{
    VecSetValue(b, 0, 2, INSERT_VALUES);
    VecSetValue(b, 1, 3, INSERT_VALUES);
    VecSetValue(b, 2, 5, INSERT_VALUES);
    VecSetValue(b, 3, 8, INSERT_VALUES);
    VecSetValue(b, 4, 13, INSERT_VALUES);
    VecSetValue(b, 5, 21, INSERT_VALUES);
    VecSetValue(b, 6, 34, INSERT_VALUES);
    VecSetValue(b, 7, 55, INSERT_VALUES);

    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    return 0;
}

PetscErrorCode
G3DTestPetscLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    for (PetscInt i = 0; i < 8; i++)
        MatSetValue(A, i, i, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}
