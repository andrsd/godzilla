#include "Godzilla.h"
#include "CallStack.h"
#include "ExplicitFVLinearProblem.h"
#include "Output.h"
#include "Validation.h"
#include "Utils.h"
#include "petscts.h"

namespace godzilla {

static PetscErrorCode
__efvlp_compute_rhs(DM dm, Real time, Vec x, Vec F, void * ctx)
{
    _F_;
    auto * efvp = static_cast<ExplicitFVLinearProblem *>(ctx);
    Vector vec_x(x);
    Vector vec_F(F);
    efvp->compute_rhs(time, vec_x, vec_F);
    return 0;
}

Parameters
ExplicitFVLinearProblem::parameters()
{
    Parameters params = NonlinearProblem::parameters();
    params += TransientProblemInterface::parameters();
    params.add_param<std::string>("scheme", "euler", "Time stepping scheme: [euler, ssp, rk]");
    return params;
}

ExplicitFVLinearProblem::ExplicitFVLinearProblem(const Parameters & params) :
    NonlinearProblem(params),
    FVProblemInterface(this, params),
    TransientProblemInterface(this, params),
    scheme(get_param<std::string>("scheme"))
{
    _F_;
    this->default_output_on = Output::ON_INITIAL | Output::ON_TIMESTEP;
}

ExplicitFVLinearProblem::~ExplicitFVLinearProblem()
{
    // we should be doing this, but DM is already gone when we try do this:
    //   DM dm = get_dm();
    //   DMTSDestroyRHSMassMatrix(dm);

    this->snes = nullptr;
    this->ksp = nullptr;
}

void
ExplicitFVLinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    PETSC_CHECK(TSGetSNES(this->ts, &this->snes));
    PETSC_CHECK(SNESGetKSP(this->snes, &this->ksp));

    FVProblemInterface::init();
}

void
ExplicitFVLinearProblem::create()
{
    _F_;
    FVProblemInterface::create();
    NonlinearProblem::create();
    TransientProblemInterface::create();
}

void
ExplicitFVLinearProblem::check()
{
    _F_;
    NonlinearProblem::check();

    if (!validation::in(this->scheme, { "euler", "ssp", "rk" }))
        log_error("The 'scheme' parameter can be either 'euler', 'ssp' or 'rk'.");
}

bool
ExplicitFVLinearProblem::converged()
{
    _F_;
    return TransientProblemInterface::converged();
}

void
ExplicitFVLinearProblem::solve()
{
    _F_;
    lprintf(9, "Solving");
    TransientProblemInterface::solve(this->x);
}

void
ExplicitFVLinearProblem::allocate_objects()
{
    _F_;
    DM dm = get_dm();
    Vec glob_x;
    PETSC_CHECK(DMCreateGlobalVector(dm, &glob_x));
    this->x = Vector(glob_x);
    this->x.set_name("sln");
    FVProblemInterface::allocate_objects();
}

void
ExplicitFVLinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this));
    PETSC_CHECK(DMTSSetRHSFunctionLocal(dm, __efvlp_compute_rhs, this));
}

void
ExplicitFVLinearProblem::set_up_initial_guess()
{
    _F_;
    lprintf(9, "Setting initial guess");
    DiscreteProblemInterface::set_up_initial_guess();
}

void
ExplicitFVLinearProblem::set_up_time_scheme()
{
    _F_;
    std::string sch = utils::to_lower(this->scheme);
    if (sch == "euler")
        PETSC_CHECK(TSSetType(this->ts, TSEULER));
    else if (sch == "ssp")
        PETSC_CHECK(TSSetType(this->ts, TSSSP));
    else if (sch == "rk")
        PETSC_CHECK(TSSetType(this->ts, TSRK));
}

void
ExplicitFVLinearProblem::set_up_monitors()
{
    _F_;
    NonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

PetscErrorCode
ExplicitFVLinearProblem::compute_rhs(Real time, const Vector & x, Vector & F)
{
    _F_;
    return DMPlexTSComputeRHSFunctionFVM(get_dm(), time, (Vec) x, (Vec) F, this);
}

} // namespace godzilla
