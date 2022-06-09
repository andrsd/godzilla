#include "Godzilla.h"
#include "CallStack.h"
#include "ExplicitFELinearProblem.h"
#include "Output.h"
#include "petscts.h"

namespace godzilla {

InputParameters
ExplicitFELinearProblem::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params += TransientProblemInterface::valid_params();
    return params;
}

ExplicitFELinearProblem::ExplicitFELinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(this, params)
{
    _F_;
    this->default_output_on = Output::ON_INITIAL | Output::ON_TIMESTEP;
}

ExplicitFELinearProblem::~ExplicitFELinearProblem()
{
    // we should be doing this, but DM is already gone when we try do this
    // DM dm = get_dm();
    // DMTSDestroyRHSMassMatrix(dm);

    this->snes = nullptr;
}

void
ExplicitFELinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    PETSC_CHECK(TSGetSNES(this->ts, &this->snes));

    FEProblemInterface::init();

    for (auto & f : this->fields) {
        PetscInt fid = f.second.id;
        PETSC_CHECK(PetscDSSetImplicit(this->ds, fid, PETSC_FALSE));
    }
}

void
ExplicitFELinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

bool
ExplicitFELinearProblem::converged()
{
    _F_;
    return TransientProblemInterface::converged();
}

void
ExplicitFELinearProblem::solve()
{
    _F_;
    TransientProblemInterface::solve(this->x);
}

void
ExplicitFELinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this));
    PETSC_CHECK(DMTSSetRHSFunctionLocal(dm, DMPlexTSComputeRHSFunctionFEM, this));
    // NOTE: this may need to be eventually in a virtual method for cases when people want to use
    // lumped mass matrix
    PETSC_CHECK(DMTSCreateRHSMassMatrix(dm));
}

void
ExplicitFELinearProblem::set_up_time_scheme()
{
    _F_;
    // TODO: allow other schemes
    PETSC_CHECK(TSSetType(this->ts, TSEULER));
}

void
ExplicitFELinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ExplicitFELinearProblem::set_residual_block(PetscInt field_id,
                                            PetscFEResidualFunc * f0,
                                            PetscFEResidualFunc * f1)
{
    _F_;
    PETSC_CHECK(PetscDSSetRHSResidual(this->ds, field_id, f0, f1));
}

} // namespace godzilla
