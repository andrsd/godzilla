#include "Godzilla.h"
#include "FunctionInterface.h"
#include "BurgersEquation.h"
#include "CallStack.h"
#include "petscdm.h"
#include "petscdmlabel.h"

using namespace godzilla;

REGISTER_OBJECT(BurgersEquation);

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
    f0[0] = -u[0] * u_x[0];
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
    PetscReal visc = constants[0];
    f1[0] = -visc * u_x[0] + 0.5 * u[0] * u[0];
}

///

InputParameters
BurgersEquation::valid_params()
{
    InputParameters params = ExplicitFELinearProblem::valid_params();
    params.add_param<PetscReal>("viscosity", "Viscosity");
    return params;
}

BurgersEquation::BurgersEquation(const InputParameters & parameters) :
    ExplicitFELinearProblem(parameters),
    viscosity(get_param<PetscReal>("viscosity"))
{
    _F_;
}

BurgersEquation::~BurgersEquation() {}

void
BurgersEquation::set_up_fields()
{
    _F_;
    add_fe(u_id, "u", 1, 1);
}

void
BurgersEquation::set_up_weak_form()
{
    _F_;
    set_residual_block(u_id, nullptr, f1_u);

    set_constants({ this->viscosity });
}
