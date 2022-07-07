#include "Godzilla.h"
#include "FunctionInterface.h"
#include "HeatEquationProblem.h"
#include "CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(HeatEquationProblem);

static void
f0_temp(PetscInt dim,
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
    PetscReal q_ppp = a[a_off[HeatEquationProblem::q_ppp_id]];
    f0[0] = u_t[0] - q_ppp;
}

static void
f1_temp(PetscInt dim,
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
    for (PetscInt d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

static void
g3_temp(PetscInt dim,
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
    for (PetscInt d = 0; d < dim; ++d)
        g3[d * dim + d] = 1.0;
}

static void
g0_temp(PetscInt dim,
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
        PetscScalar g0[])
{
    g0[0] = u_t_shift * 1.0;
}

Parameters
HeatEquationProblem::valid_params()
{
    Parameters params = ImplicitFENonlinearProblem::valid_params();
    params.add_param<PetscInt>("p_order", 1, "Polynomial order of FE space");
    return params;
}

HeatEquationProblem::HeatEquationProblem(const Parameters & parameters) :
    ImplicitFENonlinearProblem(parameters),
    p_order(get_param<PetscInt>("p_order"))
{
    _F_;
}

HeatEquationProblem::~HeatEquationProblem() {}

void
HeatEquationProblem::set_up_fields()
{
    _F_;
    add_fe(temp_id, "temp", 1, this->p_order);

    add_aux_fe(q_ppp_id, "q_ppp", 1, 0);
    add_aux_fe(htc_aux_id, "htc", 1, this->p_order);
    add_aux_fe(T_ambient_aux_id, "T_ambient", 1, this->p_order);
}

void
HeatEquationProblem::set_up_weak_form()
{
    _F_;
    set_residual_block(temp_id, f0_temp, f1_temp);
    set_jacobian_block(temp_id, temp_id, g0_temp, NULL, NULL, g3_temp);
}
