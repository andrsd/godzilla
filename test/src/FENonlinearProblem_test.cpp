#include "GodzillaConfig.h"
#include "Factory.h"
#include "Grid.h"
#include "FENonlinearProblem_test.h"
#include "InputParameters.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "petsc.h"
#include "petscvec.h"

namespace godzilla {

registerObject(GTestFENonlinearProblem);
registerObject(GTest2FieldsFENonlinearProblem);
registerObject(GTest2CompIC);

static void
f0_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f0[])
{
    f0[0] = 2.0;
}

/* gradU[comp*dim+d] = {u_x, u_y} or {u_x, u_y, u_z} */
static void
f1_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f1[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

/* < \nabla v, \nabla u + {\nabla u}^T >
   This just gives \nabla u, give the perdiagonal for the transpose */
static void
g3_uu(PetscInt dim,
      PetscInt Nf,
      PetscInt NfAux,
      const PetscInt uOff[],
      const PetscInt uOff_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt aOff[],
      const PetscInt aOff_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_tShift,
      const PetscReal x[],
      PetscInt numConstants,
      const PetscScalar constants[],
      PetscScalar g3[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        g3[d * dim + d] = 1.0;
}

TEST_F(FENonlinearProblemTest, getFieldName)
{
    grid->create();
    prob->create();

    EXPECT_EQ(prob->getFieldName(0), "u");
    EXPECT_DEATH(prob->getFieldName(1), "ERROR: Field with id = '1' does not exist.");
}

TEST_F(FENonlinearProblemTest, solve)
{
    {
        const std::string class_name = "ConstantIC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = Factory::create<InitialCondition>(class_name, "ic", params);
        prob->addInitialCondition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<std::string>>("boundary") = { "marker" };
        params.set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = Factory::create<BoundaryCondition>(class_name, "bc", params);
        prob->addBoundaryCondition(bc);
    }

    grid->create();
    prob->create();

    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    const Vec x = prob->getSolutionVector();

    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 0.25);
}

TEST_F(FENonlinearProblemTest, solve_no_ic)
{
    {
        const std::string class_name = "DirichletBC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<std::string>>("boundary") = { "marker" };
        params.set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = Factory::create<BoundaryCondition>(class_name, "bc", params);
        prob->addBoundaryCondition(bc);
    }

    grid->create();
    prob->create();

    const Vec x = prob->getSolutionVector();

    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 0.);
}

TEST_F(FENonlinearProblemTest, err_ic_comp_mismatch)
{
    {
        const std::string class_name = "GTest2CompIC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        auto ic = Factory::create<InitialCondition>(class_name, "ic", params);
        prob->addInitialCondition(ic);
    }

    grid->create();

    EXPECT_DEATH(prob->create(),
                 "ERROR: Initial condition 'ic' operates on 2 components, but is set on a field "
                 "with 1 components.");
}

TEST_F(FENonlinearProblemTest, err_duplicate_ics)
{
    {
        const std::string class_name = "ConstantIC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = Factory::create<InitialCondition>(class_name, "ic1", params);
        prob->addInitialCondition(ic);
    }

    const std::string class_name = "ConstantIC";
    InputParameters params = Factory::getValidParams(class_name);
    params.set<const App *>("_app") = this->app;
    params.set<std::vector<PetscReal>>("value") = { 0.2 };
    auto ic = Factory::create<InitialCondition>(class_name, "ic2", params);
    EXPECT_DEATH(prob->addInitialCondition(ic),
                 "ERROR: Initial condition 'ic2' is being applied to a field that already has an "
                 "initial condition.");
}

TEST(TwoFieldFENonlinearProblemTest, err_no_enough_ics)
{
    App app("godzilla", MPI_COMM_WORLD);
    Grid * grid;
    FENonlinearProblem * prob;

    {
        const std::string class_name = "LineMesh";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<PetscInt>("nx") = 2;
        grid = Factory::create<Grid>(class_name, "grid", params);
    }
    {
        const std::string class_name = "GTest2FieldsFENonlinearProblem";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<Grid *>("_grid") = grid;
        prob = Factory::create<FENonlinearProblem>(class_name, "prob", params);
    }

    {
        const std::string class_name = "ConstantIC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = &app;
        params.set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = Factory::create<InitialCondition>(class_name, "ic1", params);
        prob->addInitialCondition(ic);
    }

    grid->create();
    EXPECT_DEATH(prob->create(), "ERROR: Provided 2 field\\(s\\), but 1 initial condition\\(s\\).");
}

TEST_F(FENonlinearProblemTest, err_nonexisting_bc_bnd)
{
    {
        const std::string class_name = "DirichletBC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<std::string>>("boundary") = { "asdf" };
        params.set<std::vector<std::string>>("value") = { "0.1" };
        auto bc = Factory::create<BoundaryCondition>(class_name, "bc1", params);
        prob->addBoundaryCondition(bc);
    }

    grid->create();
    EXPECT_DEATH(prob->create(),
                 "ERROR: Boundary condition 'bc1' is set on boundary 'asdf' which does not exist "
                 "in the mesh.");
}

// GTestFENonlinearProblem

GTestFENonlinearProblem::GTestFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    u_id(-1)
{
}

GTestFENonlinearProblem::~GTestFENonlinearProblem() {}

void
GTestFENonlinearProblem::onSetFields()
{
    _F_;
    PetscInt order = 1;
    this->u_id = addField("u", 1, order);
}

void
GTestFENonlinearProblem::onSetWeakForm()
{
    setResidualBlock(this->u_id, f0_u, f1_u);
    setJacobianBlock(this->u_id, this->u_id, NULL, NULL, NULL, g3_uu);
}

//

GTest2FieldsFENonlinearProblem::GTest2FieldsFENonlinearProblem(const InputParameters & params) :
    GTestFENonlinearProblem(params)
{
}

void
GTest2FieldsFENonlinearProblem::onSetFields()
{
    GTestFENonlinearProblem::onSetFields();
    this->v_id = addField("v", 1, 1);
}

} // namespace godzilla
