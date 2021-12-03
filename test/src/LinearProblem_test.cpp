#include "gmock/gmock.h"
#include "Factory.h"
#include "Grid.h"
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
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // extract the solution and make sure it is [2, 3]
    Vec x = prob->getSolutionVector();
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
        MockLinearProblem(const InputParameters & params) : LinearProblem(params) {}

        MOCK_METHOD(void, solve, ());
        virtual bool
        converged()
        {
            return true;
        }
        MOCK_METHOD(void, output, ());
        MOCK_METHOD(PetscErrorCode, computeRhsCallback, (Vec b));
        MOCK_METHOD(PetscErrorCode, computeOperatorsCallback, (Mat A, Mat B));
    };

    auto grid = gGrid1d();
    grid->create();

    InputParameters prob_pars = LinearProblem::validParams();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<Grid *>("_grid") = grid;
    MockLinearProblem prob(prob_pars);

    EXPECT_CALL(prob, solve);
    EXPECT_CALL(prob, output);
    prob.run();
}

TEST_F(LinearProblemTest, output)
{
    class MockLinearProblem : public LinearProblem {
    public:
        MockLinearProblem(const InputParameters & params) : LinearProblem(params) {}

        virtual void
        output()
        {
            LinearProblem::output();
        }
        MOCK_METHOD(PetscErrorCode, computeRhsCallback, (Vec b));
        MOCK_METHOD(PetscErrorCode, computeOperatorsCallback, (Mat A, Mat B));
    };

    class MockOutput : public Output {
    public:
        MockOutput(const InputParameters & params) : Output(params) {}

        MOCK_METHOD(const std::string &, getFileName, (), (const));
        MOCK_METHOD(void, setFileName, ());
        MOCK_METHOD(void, setSequenceFileName, (unsigned int stepi));
        MOCK_METHOD(void, output, (DM dm, Vec vec), (const));
    };

    auto grid = gGrid1d();
    grid->create();

    InputParameters prob_pars = LinearProblem::validParams();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<Grid *>("_grid") = grid;
    MockLinearProblem prob(prob_pars);

    InputParameters out_pars = Output::validParams();
    out_pars.set<const App *>("_app") = this->app;
    out_pars.set<Problem *>("_problem") = &prob;
    MockOutput out(out_pars);

    prob.addOutput(&out);

    EXPECT_CALL(out, setFileName);
    EXPECT_CALL(out, output);

    prob.output();
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
    DM dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[2] = { 1, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G1DTestLinearProblem::computeRhsCallback(Vec b)
{
    VecSetValue(b, 0, 2, INSERT_VALUES);
    VecSetValue(b, 1, 3, INSERT_VALUES);

    VecAssemblyBegin(b);
    VecAssemblyEnd(b);

    return 0;
}

PetscErrorCode
G1DTestLinearProblem::computeOperatorsCallback(Mat A, Mat B)
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
    DM dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[3] = { 1, 0, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G2DTestLinearProblem::computeRhsCallback(Vec b)
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
G2DTestLinearProblem::computeOperatorsCallback(Mat A, Mat B)
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
    DM dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[4] = { 1, 0, 0, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    LinearProblem::create();
}

PetscErrorCode
G3DTestLinearProblem::computeRhsCallback(Vec b)
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
G3DTestLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    for (PetscInt i = 0; i < 8; i++)
        MatSetValue(A, i, i, 1, INSERT_VALUES);

    MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

    return 0;
}
