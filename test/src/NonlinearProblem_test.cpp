#include "gmock/gmock.h"
#include "Factory.h"
#include "Mesh.h"
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
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
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

    auto mesh = gMesh1d();
    mesh->create();

    InputParameters prob_pars = NonlinearProblem::validParams();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<Mesh *>("_mesh") = mesh;
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
        MOCK_METHOD(void, outputMesh, (DM dm));
        MOCK_METHOD(void, outputSolution, (Vec vec));
        MOCK_METHOD(void, outputStep, (PetscInt stepi, DM dm, Vec vec));
    };

    auto mesh = gMesh1d();
    mesh->create();

    InputParameters prob_pars = NonlinearProblem::validParams();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<Mesh *>("_mesh") = mesh;
    MockNonlinearProblem prob(prob_pars);

    InputParameters out_pars = Output::validParams();
    out_pars.set<const App *>("_app") = this->app;
    out_pars.set<Problem *>("_problem") = &prob;
    MockOutput out(out_pars);

    prob.addOutput(&out);

    EXPECT_CALL(out, outputStep);

    prob.output();
}

TEST_F(NonlinearProblemTest, line_search_type)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        MockNonlinearProblem(const InputParameters & params) : NonlinearProblem(params) {}

        MOCK_METHOD(PetscErrorCode, computeResidualCallback, (Vec x, Vec f));
        MOCK_METHOD(PetscErrorCode, computeJacobianCallback, (Vec x, Mat J, Mat Jp));

        SNES
        getSNES()
        {
            return this->snes;
        }
    };

    auto mesh = gMesh1d();
    mesh->create();

    std::vector<std::string> ls_type = { "basic", "l2", "cp", "nleqerr", "shell" };
    for (auto & lst : ls_type) {
        InputParameters prob_pars = NonlinearProblem::validParams();
        prob_pars.set<const App *>("_app") = this->app;
        prob_pars.set<Mesh *>("_mesh") = mesh;
        prob_pars.set<std::string>("line_search") = lst;
        MockNonlinearProblem prob(prob_pars);
        prob.create();

        SNES snes = prob.getSNES();
        SNESLineSearch ls;
        SNESGetLineSearch(snes, &ls);
        SNESLineSearchType tp;
        SNESLineSearchGetType(ls, &tp);
        EXPECT_STREQ(tp, lst.c_str());
    }
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
    DM dm = getDM();
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
