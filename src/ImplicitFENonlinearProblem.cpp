#include "Godzilla.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem.h"
#include "Output.h"
#include "Validation.h"
#include "Utils.h"
#include "petscts.h"

namespace godzilla {

InputParameters
ImplicitFENonlinearProblem::valid_params()
{
    InputParameters params = FENonlinearProblem::valid_params();
    params += TransientProblemInterface::valid_params();
    params.add_param<std::string>("scheme", "beuler", "Time stepping scheme: [beuler, cn]");
    return params;
}

ImplicitFENonlinearProblem::ImplicitFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientProblemInterface(this, params),
    scheme(get_param<std::string>("scheme"))
{
    _F_;
    this->default_output_on = Output::ON_INITIAL | Output::ON_TIMESTEP;
}

ImplicitFENonlinearProblem::~ImplicitFENonlinearProblem()
{
    this->snes = nullptr;
}

void
ImplicitFENonlinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    PETSC_CHECK(TSGetSNES(this->ts, &this->snes));

    FEProblemInterface::init();
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientProblemInterface::create();
}

void
ImplicitFENonlinearProblem::check()
{
    _F_;
    FENonlinearProblem::check();

    if (!validation::in(this->scheme, {"beuler", "cn"}))
        log_error("The 'scheme' parameter can be either 'beuler' or 'cn'.");
}

bool
ImplicitFENonlinearProblem::converged()
{
    _F_;
    return TransientProblemInterface::converged();
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    TransientProblemInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this));
    PETSC_CHECK(DMTSSetIFunctionLocal(dm, DMPlexTSComputeIFunctionFEM, this));
    PETSC_CHECK(DMTSSetIJacobianLocal(dm, DMPlexTSComputeIJacobianFEM, this));
}

void
ImplicitFENonlinearProblem::set_up_time_scheme()
{
    _F_;
    std::string sch = utils::to_lower(this->scheme);
    if (sch.compare("beuler") == 0)
        PETSC_CHECK(TSSetType(this->ts, TSBEULER));
    else if (sch.compare("cn") == 0)
        PETSC_CHECK(TSSetType(this->ts, TSCN));
}

void
ImplicitFENonlinearProblem::set_up_monitors()
{
    _F_;
    FENonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

} // namespace godzilla
