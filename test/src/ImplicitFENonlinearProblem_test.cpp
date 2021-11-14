#include "GodzillaConfig.h"
#include "Factory.h"
#include "Grid.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "InputParameters.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "petsc.h"
#include "petscvec.h"

namespace godzilla {

registerObject(GTestImplicitFENonlinearProblem);

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
    f0[0] = u_t[0];
}

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

static void
g0_uu(PetscInt dim,
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
    g3[0] = 1.0 * u_tShift;
}

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

TEST_F(ImplicitFENonlinearProblemTest, run)
{
    {
        const std::string class_name = "ConstantIC";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<PetscReal>>("value") = { 0 };
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

    prob->run();

    const Vec x = prob->getSolutionVector();

    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);

    EXPECT_NEAR(xx[0], 0.5, 1e-7);
}

// GTestImplicitFENonlinearProblem

GTestImplicitFENonlinearProblem::GTestImplicitFENonlinearProblem(const InputParameters & params) :
    ImplicitFENonlinearProblem(params),
    u_id(-1)
{
}

GTestImplicitFENonlinearProblem::~GTestImplicitFENonlinearProblem() {}

void
GTestImplicitFENonlinearProblem::onSetFields()
{
    _F_;
    PetscInt order = 1;
    this->u_id = addField("u", 1, order);
}

void
GTestImplicitFENonlinearProblem::onSetWeakForm()
{
    setResidualBlock(this->u_id, f0_u, f1_u);
    setJacobianBlock(this->u_id, this->u_id, g0_uu, NULL, NULL, g3_uu);
}

} // namespace godzilla
