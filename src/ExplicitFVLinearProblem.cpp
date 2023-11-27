#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExplicitFVLinearProblem.h"
#include "godzilla/Output.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
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
    params.add_param<std::string>("scheme",
                                  "euler",
                                  "Time stepping scheme: [euler, ssp-rk-2, ssp-rk-3, rk-2, heun]");
    return params;
}

ExplicitFVLinearProblem::ExplicitFVLinearProblem(const Parameters & params) :
    NonlinearProblem(params),
    FVProblemInterface(this, params),
    TransientProblemInterface(this, params),
    scheme(get_param<std::string>("scheme"))
{
    _F_;
    set_default_output_on(ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
}

ExplicitFVLinearProblem::~ExplicitFVLinearProblem()
{
    this->M.destroy();
    this->M_lumped_inv.destroy();
    set_snes(nullptr);
}

Real
ExplicitFVLinearProblem::get_time() const
{
    _F_;
    return TransientProblemInterface::get_time();
}

Int
ExplicitFVLinearProblem::get_step_num() const
{
    _F_;
    return TransientProblemInterface::get_step_number();
}

const std::string &
ExplicitFVLinearProblem::get_scheme() const
{
    _F_;
    return this->scheme;
}

void
ExplicitFVLinearProblem::init()
{
    _F_;
    TransientProblemInterface::init();
    auto snes = TransientProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
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

    if (!validation::in(this->scheme, { "euler", "ssp-rk-2", "ssp-rk-3", "rk-2", "heun" }))
        log_error("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', 'ssp-rk-3', 'rk-2' or "
                  "'heun'.");
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
    lprint(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
}

void
ExplicitFVLinearProblem::build_local_solution_vector(godzilla::Vector & loc_sln)
{
    _F_;
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitFVLinearProblem::allocate_objects()
{
    _F_;
    Problem::allocate_objects();
    FVProblemInterface::allocate_objects();
}

void
ExplicitFVLinearProblem::set_up_callbacks()
{
    _F_;
    PETSC_CHECK(DMTSSetRHSFunction(get_dm(), __efvlp_compute_rhs, this));
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
    std::string name = utils::to_lower(this->scheme);
    std::map<std::string, TimeScheme> scheme_map = { { "euler", TimeScheme::EULER },
                                                     { "ssp-rk-2", TimeScheme::SSP_RK_2 },
                                                     { "ssp-rk-3", TimeScheme::SSP_RK_3 },
                                                     { "rk-2", TimeScheme::RK_2 },
                                                     { "heun", TimeScheme::HEUN } };
    set_scheme(scheme_map[name]);
}

void
ExplicitFVLinearProblem::set_up_monitors()
{
    _F_;
    NonlinearProblem::set_up_monitors();
    TransientProblemInterface::set_up_monitors();
}

void
ExplicitFVLinearProblem::allocate_mass_matrix()
{
    _F_;
    this->M = create_matrix();
    set_ksp_operators(this->M, this->M);
}

void
ExplicitFVLinearProblem::allocate_lumped_mass_matrix()
{
    _F_;
    this->M_lumped_inv = create_global_vector();
}

void
ExplicitFVLinearProblem::create_mass_matrix()
{
    _F_;
    Mat m;
    PETSC_CHECK(DMCreateMassMatrix(get_dm(), get_dm(), &m));
    this->M = Matrix(m);
    set_ksp_operators(this->M, this->M);
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
    Vector loc_x = get_local_vector();
    Vector loc_F = get_local_vector();
    loc_x.zero();
    compute_boundary_local(time, loc_x);
    PETSC_CHECK(DMGlobalToLocal(get_dm(), x, INSERT_VALUES, loc_x));
    loc_F.zero();
    compute_rhs_local(time, loc_x, loc_F);
    F.zero();
    PETSC_CHECK(DMLocalToGlobal(get_dm(), loc_F, ADD_VALUES, F));
    if ((Vec) this->M_lumped_inv == nullptr) {
        auto ksp = get_ksp();
        PETSC_CHECK(KSPSolve(ksp, F, F));
    }
    else
        Vector::pointwise_mult(F, this->M_lumped_inv, F);
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

void
ExplicitFVLinearProblem::post_step()
{
    _F_;
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(ExecuteOn::TIMESTEP);
}

} // namespace godzilla
