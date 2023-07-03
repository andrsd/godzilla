#include "Godzilla.h"
#include "CallStack.h"
#include "ExplicitFVLinearProblem.h"
#include "Output.h"
#include "Validation.h"
#include "Utils.h"
#include "petscts.h"

namespace godzilla {

static PetscErrorCode
__efvlp_compute_rhs(TS, Real time, Vec x, Vec F, void * ctx)
{
    _F_;
    auto * efvp = static_cast<ExplicitFVLinearProblem *>(ctx);
    Vector vec_x(x);
    Vector vec_F(F);
    return efvp->compute_rhs(time, vec_x, vec_F);
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
    this->M.destroy();
    this->M_lumped_inv.destroy();
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
    TransientProblemInterface::check(this);

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
    this->x = create_global_vector();
    this->x.set_name("sln");
    FVProblemInterface::allocate_objects();
}

void
ExplicitFVLinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMTSSetRHSFunction(dm, __efvlp_compute_rhs, this));
}

void
ExplicitFVLinearProblem::set_up_initial_guess()
{
    _F_;
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
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

void
ExplicitFVLinearProblem::create_mass_matrix()
{
    _F_;
    DM dm = get_dm();
    Mat m;
    PETSC_CHECK(DMCreateMassMatrix(dm, dm, &m));
    this->M = Matrix(m);
    PETSC_CHECK(KSPSetOperators(this->ksp, this->M, this->M));
}

void
ExplicitFVLinearProblem::create_mass_matrix_lumped()
{
    _F_;
    Vec v;
    PETSC_CHECK(DMCreateMassMatrixLumped(get_dm(), &v));
    this->M_lumped_inv = Vector(v);
    this->M_lumped_inv.reciprocal();
}

PetscErrorCode
ExplicitFVLinearProblem::compute_rhs(Real time, const Vector & x, Vector & F)
{
    _F_;
    DM dm = get_dm();
    Vector loc_x = get_local_vector();
    Vector loc_F = get_local_vector();
    loc_x.zero();
    compute_boundary_local(time, loc_x);
    PETSC_CHECK(DMGlobalToLocal(dm, x, INSERT_VALUES, loc_x));
    loc_F.zero();
    compute_rhs_local(time, loc_x, loc_F);
    F.zero();
    PETSC_CHECK(DMLocalToGlobal(dm, loc_F, ADD_VALUES, F));
    if ((Vec) this->M_lumped_inv == nullptr)
        PETSC_CHECK(KSPSolve(this->ksp, F, F));
    else
        PETSC_CHECK(VecPointwiseMult(F, this->M_lumped_inv, F));
    restore_local_vector(loc_x);
    restore_local_vector(loc_F);
    return 0;
}

PetscErrorCode
ExplicitFVLinearProblem::compute_boundary_local(Real time, Vector & x)
{
    _F_;
    return DMPlexTSComputeBoundary(get_dm(), time, x, nullptr, this);
}

PetscErrorCode
ExplicitFVLinearProblem::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
    _F_;
    return DMPlexTSComputeRHSFunctionFVM(get_dm(), time, x, F, this);
}

} // namespace godzilla
