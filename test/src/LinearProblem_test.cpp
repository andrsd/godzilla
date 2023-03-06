#include "gmock/gmock.h"
#include "Factory.h"
#include "Mesh.h"
#include "LinearProblem_test.h"
#include "Parameters.h"
#include "Output.h"
#include "petsc.h"
#include "petscmat.h"

using namespace godzilla;

REGISTER_OBJECT(G1DTestLinearProblem);
REGISTER_OBJECT(G2DTestLinearProblem);
REGISTER_OBJECT(G3DTestLinearProblem);

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
    auto x = prob->get_solution_vector();
    Int ni = 2;
    Int ix[2] = { 0, 1 };
    Scalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}

TEST_F(LinearProblemTest, run)
{
    class MockLinearProblem : public LinearProblem {
    public:
        explicit MockLinearProblem(const Parameters & params) : LinearProblem(params) {}

        MOCK_METHOD(void, solve, ());
        virtual bool
        converged()
        {
            return true;
        }
        MOCK_METHOD(void, on_final, ());
        MOCK_METHOD(PetscErrorCode, compute_rhs, (Vector & b));
        MOCK_METHOD(PetscErrorCode, compute_operators, (Matrix & A, Matrix & B));
    };

    auto mesh = gMesh1d();
    mesh->create();

    Parameters prob_pars = LinearProblem::parameters();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = mesh;
    MockLinearProblem prob(prob_pars);

    EXPECT_CALL(prob, solve);
    EXPECT_CALL(prob, on_final);
    prob.run();
}

// 1D

G1DTestLinearProblem::G1DTestLinearProblem(const Parameters & params) : LinearProblem(params) {}

G1DTestLinearProblem::~G1DTestLinearProblem()
{
    this->s.destroy();
}

void
G1DTestLinearProblem::create()
{
    DM dm = get_dm();
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    this->s = Section::create(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL);
    this->mesh->set_local_section(this->s);
    LinearProblem::create();
}

PetscErrorCode
G1DTestLinearProblem::compute_rhs(Vector & b)
{
    b.set_value(0, 2);
    b.set_value(1, 3);

    b.assembly_begin();
    b.assembly_end();

    return 0;
}

PetscErrorCode
G1DTestLinearProblem::compute_operators(Matrix & A, Matrix & B)
{
    A.set_value(0, 0, 1.);
    A.set_value(1, 1, 1.);

    A.assembly_begin();
    A.assembly_end();

    return 0;
}

// 2D

G2DTestLinearProblem::G2DTestLinearProblem(const Parameters & params) : LinearProblem(params) {}

G2DTestLinearProblem::~G2DTestLinearProblem()
{
    this->s.destroy();
}

void
G2DTestLinearProblem::create()
{
    DM dm = get_dm();
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[3] = { 1, 0, 0 };
    this->s = Section::create(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL);
    this->mesh->set_local_section(this->s);
    LinearProblem::create();
}

PetscErrorCode
G2DTestLinearProblem::compute_rhs(Vector & b)
{
    b.set_values({ 0, 1, 2, 3 }, { 2, 3, 5, 8 });

    b.assembly_begin();
    b.assembly_end();

    return 0;
}

PetscErrorCode
G2DTestLinearProblem::compute_operators(Matrix & A, Matrix & B)
{
    for (Int i = 0; i < 4; i++)
        A.set_value(i, i, 1.);

    A.assembly_begin();
    A.assembly_end();

    return 0;
}

// 3D

G3DTestLinearProblem::G3DTestLinearProblem(const Parameters & params) : LinearProblem(params) {}

G3DTestLinearProblem::~G3DTestLinearProblem()
{
    this->s.destroy();
}

void
G3DTestLinearProblem::create()
{
    DM dm = get_dm();
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[4] = { 1, 0, 0, 0 };
    this->s = Section::create(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL);
    this->mesh->set_local_section(this->s);
    LinearProblem::create();
}

PetscErrorCode
G3DTestLinearProblem::compute_rhs(Vector & b)
{
    b.set_values({ 0, 1, 2, 3, 4, 5, 6, 7 }, { 2, 3, 5, 8, 13, 21, 34, 55 });

    b.assembly_begin();
    b.assembly_end();

    return 0;
}

PetscErrorCode
G3DTestLinearProblem::compute_operators(Matrix & A, Matrix & B)
{
    for (Int i = 0; i < 8; i++)
        A.set_value(i, i, 1.);

    A.assembly_begin();
    A.assembly_end();

    return 0;
}
