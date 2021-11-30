#include "gmock/gmock.h"
#include "Factory.h"
#include "Grid.h"
#include "NonlinearProblem_test.h"
#include "InputParameters.h"
#include "Output.h"
#include "petsc.h"
#include "petscvec.h"
#include "petscmat.h"

using namespace godzilla;

registerObject(G1DTestNonlinearProblem);

TEST_F(NonlinearProblemTest, solve)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
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

TEST_F(NonlinearProblemTest, run)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        MockNonlinearProblem(const InputParameters & params) : NonlinearProblem(params) {}

        MOCK_METHOD(void, solve, ());
        virtual bool
        converged()
        {
            return true;
        }
        MOCK_METHOD(void, output, ());
        MOCK_METHOD(PetscErrorCode, computeResidualCallback, (Vec x, Vec f));
        MOCK_METHOD(PetscErrorCode, computeJacobianCallback, (Vec x, Mat J, Mat Jp));
    };

    auto grid = gGrid1d();
    grid->create();

    InputParameters prob_pars = NonlinearProblem::validParams();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<Grid *>("_grid") = grid;
    MockNonlinearProblem prob(prob_pars);

    EXPECT_CALL(prob, solve);
    EXPECT_CALL(prob, output);
    prob.run();
}

TEST_F(NonlinearProblemTest, output)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        MockNonlinearProblem(const InputParameters & params) : NonlinearProblem(params) {}

        virtual void
        output()
        {
            NonlinearProblem::output();
        }
        MOCK_METHOD(PetscErrorCode, computeResidualCallback, (Vec x, Vec f));
        MOCK_METHOD(PetscErrorCode, computeJacobianCallback, (Vec x, Mat J, Mat Jp));
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

    InputParameters prob_pars = NonlinearProblem::validParams();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<Grid *>("_grid") = grid;
    MockNonlinearProblem prob(prob_pars);

    InputParameters out_pars = Output::validParams();
    out_pars.set<const App *>("_app") = this->app;
    out_pars.set<Problem *>("_problem") = &prob;
    MockOutput out(out_pars);

    prob.addOutput(&out);

    EXPECT_CALL(out, setFileName);
    EXPECT_CALL(out, output);

    prob.output();
}

//

G1DTestNonlinearProblem::G1DTestNonlinearProblem(const InputParameters & params) :
    NonlinearProblem(params),
    s(nullptr)
{
}

G1DTestNonlinearProblem::~G1DTestNonlinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G1DTestNonlinearProblem::create()
{
    const DM & dm = getDM();
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[2] = { 1, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    NonlinearProblem::create();
}

PetscErrorCode
G1DTestNonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    PetscInt ni = 2;
    PetscInt ix[] = { 0, 1 };
    PetscScalar y[2];
    VecGetValues(x, ni, ix, y);

    VecSetValue(f, 0, y[0] - 2, INSERT_VALUES);
    VecSetValue(f, 1, y[1] - 3, INSERT_VALUES);

    VecAssemblyBegin(f);
    VecAssemblyEnd(f);

    return 0;
}

PetscErrorCode
G1DTestNonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    MatSetValue(J, 0, 0, 1, INSERT_VALUES);
    MatSetValue(J, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(J, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(J, MAT_FINAL_ASSEMBLY);

    return 0;
}
