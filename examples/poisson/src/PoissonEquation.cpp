#include "Godzilla.h"
#include "FunctionInterface.h"
#include "PoissonEquation.h"
#include "CallStack.h"
#include "petscdm.h"
#include "petscdmlabel.h"

using namespace godzilla;

registerObject(PoissonEquation);

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
PoissonEquation::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params.add_param<PetscInt>("p_order", 1., "Polynomial order of the FE space.");
    return params;
}

PoissonEquation::PoissonEquation(const InputParameters & parameters) :
    FENonlinearProblem(parameters),
    p_order(get_param<PetscInt>("p_order")),
    iu(0),
    affn(0)
{
    _F_;
}

PoissonEquation::~PoissonEquation() {}

void
PoissonEquation::set_up_fields()
{
    _F_;
    add_fe(this->iu, "u", 1, this->p_order);
    add_aux_fe(this->affn, "forcing_fn", 1, this->p_order);
}

void
PoissonEquation::set_up_weak_form()
{
    _F_;
    set_residual_block(this->iu, f0_u, f1_u);
    set_jacobian_block(this->iu, this->iu, NULL, NULL, NULL, g3_uu);
}
