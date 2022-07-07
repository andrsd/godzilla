#include "Godzilla.h"
#include "HeatEquationProblem.h"
#include "ConvectiveHeatFluxBC.h"
#include "CallStack.h"

namespace godzilla {

REGISTER_OBJECT(ConvectiveHeatFluxBC);

void
__f0_convective_heat_flux_const_bc(PetscInt dim,
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
                                   const PetscReal n[],
                                   PetscInt num_constants,
                                   const PetscScalar constants[],
                                   PetscScalar f0[])
{
    PetscReal htc = a[a_off[HeatEquationProblem::htc_aux_id]];
    PetscReal T_infinity = a[a_off[HeatEquationProblem::T_ambient_aux_id]];
    f0[0] = htc * (u[0] - T_infinity);
}

void
__g0_convective_heat_flux_const_bc(PetscInt dim,
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
                                   const PetscReal n[],
                                   PetscInt num_constants,
                                   const PetscScalar constants[],
                                   PetscScalar g0[])
{
    PetscReal htc = a[a_off[HeatEquationProblem::htc_aux_id]];
    g0[0] = htc;
}

Parameters
ConvectiveHeatFluxBC::valid_params()
{
    Parameters params = NaturalBC::valid_params();
    return params;
}

ConvectiveHeatFluxBC::ConvectiveHeatFluxBC(const Parameters & params) : NaturalBC(params)
{
    _F_;
}

PetscInt
ConvectiveHeatFluxBC::get_num_components() const
{
    return 1;
}

std::vector<PetscInt>
ConvectiveHeatFluxBC::get_components() const
{
    std::vector<PetscInt> comps = { 0 };
    return comps;
}

void
ConvectiveHeatFluxBC::set_up_weak_form()
{
    _F_;
    set_residual_block(__f0_convective_heat_flux_const_bc, nullptr);
    set_jacobian_block(get_field_id(),
                       __g0_convective_heat_flux_const_bc,
                       nullptr,
                       nullptr,
                       nullptr);
}

} // namespace godzilla
