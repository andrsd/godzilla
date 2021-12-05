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
     PetscInt nf,
     PetscInt nf_aux,
     const PetscInt u_off[],
     const PetscInt u_off_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt a_off[],
     const PetscInt a_off_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt num_constants,
     const PetscScalar constants[],
     PetscScalar f0[])
{
    f0[0] = -a[0];
}

static void
f1_u(PetscInt dim,
     PetscInt nf,
     PetscInt nf_aux,
     const PetscInt u_off[],
     const PetscInt u_off_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt a_off[],
     const PetscInt a_off_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt num_constants,
     const PetscScalar constants[],
     PetscScalar f1[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

static void
g3_uu(PetscInt dim,
      PetscInt nf,
      PetscInt nf_aux,
      const PetscInt u_off[],
      const PetscInt u_off_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt a_off[],
      const PetscInt a_off_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_t_shift,
      const PetscReal x[],
      PetscInt num_constants,
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
    params.addParam<PetscInt>("p_order", 1., "Polynomial order of the FE space.");
    return params;
}

PoissonFENonlinearProblem::PoissonFENonlinearProblem(const InputParameters & parameters) :
    FENonlinearProblem(parameters),
    p_order(getParam<PetscInt>("p_order")),
    iu(0),
    affn(0)
{
    _F_;
}

PoissonFENonlinearProblem::~PoissonFENonlinearProblem() {}

void
PoissonFENonlinearProblem::onSetFields()
{
    _F_;
    addFE(this->iu, "u", 1, this->p_order);
    addAuxFE(this->affn, "forcing_fn", 1, this->p_order);
}

void
PoissonFENonlinearProblem::onSetWeakForm()
{
    _F_;
    setResidualBlock(this->iu, f0_u, f1_u);
    setJacobianBlock(this->iu, this->iu, NULL, NULL, NULL, g3_uu);
}
