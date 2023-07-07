#include "Godzilla.h"
#include "FunctionInterface.h"
#include "AdvectionEquation.h"
#include "CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(AdvectionEquation);

///

namespace {

void
compute_flux(Int dim,
             Int nf,
             const Real x[],
             const Real n[],
             const Scalar uL[],
             const Scalar uR[],
             Int n_consts,
             const Scalar constants[],
             Scalar flux[],
             void * ctx)
{
    _F_;
    auto * ae = static_cast<AdvectionEquation *>(ctx);
    ae->compute_flux(dim, nf, x, n, uL, uR, n_consts, constants, flux);
}

} // namespace

Parameters
AdvectionEquation::parameters()
{
    Parameters params = ExplicitFVLinearProblem::parameters();
    return params;
}

AdvectionEquation::AdvectionEquation(const Parameters & parameters) :
    ExplicitFVLinearProblem(parameters)
{
    _F_;
}

void
AdvectionEquation::create()
{
    _F_;
    ExplicitFVLinearProblem::create();
    create_mass_matrix();
}

void
AdvectionEquation::set_up_fields()
{
    _F_;
    add_field(0, "u", 1);
}

void
AdvectionEquation::set_up_ds()
{
    _F_;
    ExplicitFVLinearProblem::set_up_ds();
    PETSC_CHECK(PetscDSSetRiemannSolver(this->ds, 0, ::compute_flux));
    PETSC_CHECK(PetscDSSetContext(this->ds, 0, this));
}

void
AdvectionEquation::compute_flux(PetscInt dim,
                                PetscInt nf,
                                const PetscReal x[],
                                const PetscReal n[],
                                const PetscScalar uL[],
                                const PetscScalar uR[],
                                PetscInt n_consts,
                                const PetscScalar constants[],
                                PetscScalar flux[])
{
    _F_;
    PetscReal wind[] = { 0.5 };
    PetscReal wn = 0;
    wn += wind[0] * n[0];
    flux[0] = (wn > 0 ? uL[0] : uR[0]) * wn;
}
