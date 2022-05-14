#include "Godzilla.h"
#include "ConvectiveHeatFluxConstBC.h"
#include "CallStack.h"

namespace godzilla {

registerObject(ConvectiveHeatFluxConstBC);

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
    PetscReal htc = 100;
    PetscReal T_infinity = 400;
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
    PetscReal htc = 100;
    g0[0] = htc;
}

InputParameters
ConvectiveHeatFluxConstBC::valid_params()
{
    InputParameters params = NaturalBC::valid_params();
    params.add_required_param<PetscReal>("htc", "Convective heat transfer coefficient");
    params.add_required_param<PetscReal>("T_infinity", "Ambient temperature");
    return params;
}

ConvectiveHeatFluxConstBC::ConvectiveHeatFluxConstBC(const InputParameters & params) :
    NaturalBC(params),
    htc(get_param<PetscReal>("htc")),
    T_infinity(get_param<PetscReal>("T_infinity"))
{
    _F_;
}

PetscInt
ConvectiveHeatFluxConstBC::get_field_id() const
{
    return 0;
}

PetscInt
ConvectiveHeatFluxConstBC::get_num_components() const
{
    return 1;
}

std::vector<PetscInt>
ConvectiveHeatFluxConstBC::get_components() const
{
    std::vector<PetscInt> comps = { 0 };
    return comps;
}

void
ConvectiveHeatFluxConstBC::on_set_weak_form()
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
