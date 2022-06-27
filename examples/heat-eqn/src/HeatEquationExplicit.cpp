#include "Godzilla.h"
#include "FunctionInterface.h"
#include "HeatEquationExplicit.h"
#include "CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(HeatEquationExplicit);

static void
f0_temp(PetscInt dim,
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
    PetscReal f = a[0];
    f0[0] = -f;
}

static void
f1_temp(PetscInt dim,
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
    for (PetscInt d = 0; d < dim; ++d)
        f1[d] = -u_x[d];
}

InputParameters
HeatEquationExplicit::valid_params()
{
    InputParameters params = ExplicitFELinearProblem::valid_params();
    params.add_param<PetscInt>("order", 1, "Polynomial order of the FE space");
    return params;
}

HeatEquationExplicit::HeatEquationExplicit(const InputParameters & parameters) :
    ExplicitFELinearProblem(parameters),
    order(get_param<PetscInt>("order"))
{
    _F_;
}

HeatEquationExplicit::~HeatEquationExplicit() {}

void
HeatEquationExplicit::set_up_fields()
{
    _F_;
    add_fe(temp_id, "temp", 1, this->order);
    add_aux_fe(ffn_aux_id, "forcing_fn", 1, this->order);
}

void
HeatEquationExplicit::set_up_weak_form()
{
    _F_;
    set_residual_block(temp_id, f0_temp, f1_temp);
}
