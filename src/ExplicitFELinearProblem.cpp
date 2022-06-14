#include "Godzilla.h"
#include "CallStack.h"
#include "ExplicitFELinearProblem.h"
#include "Output.h"
#include "Validation.h"
#include "Utils.h"
#include "petscts.h"

namespace godzilla {

InputParameters
ExplicitFELinearProblem::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params += TransientProblemInterface::valid_params();
    params.add_param<std::string>("scheme", "euler", "Time stepping scheme: [euler, ssp, rk]");
    return params;
}

ExplicitFELinearProblem::ExplicitFELinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(this, params),
    scheme(get_param<std::string>("scheme"))
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
    this->ksp = nullptr;
}

void
ExplicitFELinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    PETSC_CHECK(TSGetSNES(this->ts, &this->snes));
    PETSC_CHECK(SNESGetKSP(this->snes, &this->ksp));

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

void
ExplicitFELinearProblem::check()
{
    _F_;
    FENonlinearProblem::check();

    if (!validation::in(this->scheme, { "euler", "ssp", "rk" }))
        log_error("The 'scheme' parameter can be either 'euler', 'ssp' or 'rk'.");
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
    lprintf(9, "Solving");
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
    std::string sch = utils::to_lower(this->scheme);
    if (sch.compare("beuler") == 0)
        PETSC_CHECK(TSSetType(this->ts, TSEULER));
    else if (sch.compare("ssp") == 0)
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
    else if (sch.compare("rk") == 0)
        PETSC_CHECK(TSSetType(this->ts, TSRK));
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
