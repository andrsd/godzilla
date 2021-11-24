#include "Godzilla.h"
#include "FunctionInterface.h"
#include "PoissonFENonlinearProblem.h"
#include "CallStack.h"
#include "petscdm.h"
#include "petscdmlabel.h"

using namespace godzilla;

registerObject(PoissonFENonlinearProblem);

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
    f0[0] = -constants[0];
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

///

InputParameters
PoissonFENonlinearProblem::validParams()
{
    InputParameters params = FENonlinearProblem::validParams();
    params.addRequiredParam<PetscReal>("forcing_fn", "Forcing function.");
    return params;
}

PoissonFENonlinearProblem::PoissonFENonlinearProblem(const InputParameters & parameters) :
    FENonlinearProblem(parameters),
    ffn(getParam<PetscReal>("forcing_fn"))
{
    _F_;
}

PoissonFENonlinearProblem::~PoissonFENonlinearProblem() {}

void
PoissonFENonlinearProblem::onSetFields()
{
    _F_;
    PetscInt order = 1;
    this->u_id = addField("u", 1, order);
}

void
PoissonFENonlinearProblem::onSetWeakForm()
{
    setResidualBlock(this->u_id, f0_u, f1_u);
    setJacobianBlock(this->u_id, this->u_id, NULL, NULL, NULL, g3_uu);

    addConstant(this->ffn);
}
