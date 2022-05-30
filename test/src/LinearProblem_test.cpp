#include "gmock/gmock.h"
#include "Factory.h"
#include "Mesh.h"
#include "LinearProblem_test.h"
#include "InputParameters.h"
#include "Output.h"
#include "petsc.h"
#include "petscmat.h"

using namespace godzilla;

registerObject(G1DTestLinearProblem);
registerObject(G2DTestLinearProblem);
registerObject(G3DTestLinearProblem);

//

TEST_F(LinearProblemTest, solve)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // extract the solution and make sure it is [2, 3]
    Vec x = prob->get_solution_vector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}

TEST_F(LinearProblemTest, run)
{
    class MockLinearProblem : public LinearProblem {
    public:
        explicit MockLinearProblem(const InputParameters & params) : LinearProblem(params) {}

        MOCK_METHOD(void, solve, ());
        virtual bool
        converged()
        {
            return true;
        }
        MOCK_METHOD(void, output_final, ());
        MOCK_METHOD(PetscErrorCode, compute_rhs_callback, (Vec b));
        MOCK_METHOD(PetscErrorCode, compute_operators_callback, (Mat A, Mat B));
    };

    auto mesh = gMesh1d();
    mesh->create();

    InputParameters prob_pars = LinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = mesh;
    MockLinearProblem prob(prob_pars);

    EXPECT_CALL(prob, solve);
    EXPECT_CALL(prob, output_final);
    prob.run();
}

// 1D

G1DTestLinearProblem::G1DTestLinearProblem(const InputParameters & params) :
    LinearProblem(params),
    s(nullptr)
{
}

G1DTestLinearProblem::~G1DTestLinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G1DTestLinearProblem::create()
{
    DM dm = get_dm();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[2] = { 1, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G1DTestLinearProblem::compute_rhs_callback(Vec b)
{
    VecSetValue(b, 0, 2, INSERT_VALUES);
    VecSetValue(b, 1, 3, INSERT_VALUES);

    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    return 0;
}

PetscErrorCode
G1DTestLinearProblem::compute_operators_callback(Mat A, Mat B)
{
    MatSetValue(A, 0, 0, 1, INSERT_VALUES);
    MatSetValue(A, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}

// 2D

G2DTestLinearProblem::G2DTestLinearProblem(const InputParameters & params) :
    LinearProblem(params),
    s(nullptr)
{
}

G2DTestLinearProblem::~G2DTestLinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G2DTestLinearProblem::create()
{
    DM dm = get_dm();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[3] = { 1, 0, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G2DTestLinearProblem::compute_rhs_callback(Vec b)
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
G2DTestLinearProblem::compute_operators_callback(Mat A, Mat B)
{
    for (PetscInt i = 0; i < 4; i++)
        MatSetValue(A, i, i, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}

// 3D

G3DTestLinearProblem::G3DTestLinearProblem(const InputParameters & params) :
    LinearProblem(params),
    s(nullptr)
{
}

G3DTestLinearProblem::~G3DTestLinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G3DTestLinearProblem::create()
{
    DM dm = get_dm();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[4] = { 1, 0, 0, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G3DTestLinearProblem::compute_rhs_callback(Vec b)
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
G3DTestLinearProblem::compute_operators_callback(Mat A, Mat B)
{
    for (PetscInt i = 0; i < 8; i++)
        MatSetValue(A, i, i, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}
