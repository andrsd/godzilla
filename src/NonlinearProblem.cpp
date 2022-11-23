#include "NonlinearProblem.h"
#include "CallStack.h"
#include "Utils.h"
#include "Mesh.h"
#include "Output.h"
#include "Validation.h"
#include "petscds.h"

namespace godzilla {

PetscErrorCode
__compute_residual(SNES snes, Vec x, Vec f, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->compute_residual_callback(x, f);
}

PetscErrorCode
__compute_jacobian(SNES snes, Vec x, Mat J, Mat Jp, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->compute_jacobian_callback(x, J, Jp);
}

PetscErrorCode
__ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

PetscErrorCode
__snes_monitor(SNES snes, PetscInt it, PetscReal norm, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->snes_monitor_callback(it, norm);
}

Parameters
NonlinearProblem::parameters()
{
    Parameters params = Problem::parameters();
    params.add_param<std::string>("line_search", "bt", "The type of line search to be used");
    params.add_param<PetscReal>("nl_rel_tol",
                                1e-8,
                                "Relative convergence tolerance for the non-linear solver");
    params.add_param<PetscReal>("nl_abs_tol",
                                1e-15,
                                "Absolute convergence tolerance for the non-linear solver");
    params.add_param<PetscReal>(
        "nl_step_tol",
        1e-15,
        "Convergence tolerance in terms of the norm of the change in the solution between steps");
    params.add_param<PetscInt>("nl_max_iter",
                               40,
                               "Maximum number of iterations for the non-linear solver");
    params.add_param<PetscReal>("lin_rel_tol",
                                1e-5,
                                "Relative convergence tolerance for the linear solver");
    params.add_param<PetscReal>("lin_abs_tol",
                                1e-50,
                                "Absolute convergence tolerance for the linear solver");
    params.add_param<PetscInt>("lin_max_iter",
                               10000,
                               "Maximum number of iterations for the linear solver");
    return params;
}

NonlinearProblem::NonlinearProblem(const Parameters & parameters) :
    Problem(parameters),
    snes(nullptr),
    ksp(nullptr),
    x(nullptr),
    r(nullptr),
    J(nullptr),
    converged_reason(SNES_CONVERGED_ITERATING),
    line_search_type(get_param<std::string>("line_search")),
    nl_rel_tol(get_param<PetscReal>("nl_rel_tol")),
    nl_abs_tol(get_param<PetscReal>("nl_abs_tol")),
    nl_step_tol(get_param<PetscReal>("nl_step_tol")),
    nl_max_iter(get_param<PetscInt>("nl_max_iter")),
    lin_rel_tol(get_param<PetscReal>("lin_rel_tol")),
    lin_abs_tol(get_param<PetscReal>("lin_abs_tol")),
    lin_max_iter(get_param<PetscInt>("lin_max_iter"))
{
    _F_;
    this->default_output_on = Output::ON_FINAL;
    line_search_type = utils::to_lower(line_search_type);
}

NonlinearProblem::~NonlinearProblem()
{
    _F_;
    if (this->snes)
        SNESDestroy(&this->snes);
    if (this->r)
        VecDestroy(&this->r);
    if (this->x)
        VecDestroy(&this->x);
    if (this->J)
        MatDestroy(&this->J);
}

DM
NonlinearProblem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

Vec
NonlinearProblem::get_solution_vector() const
{
    _F_;
    return this->x;
}

void
NonlinearProblem::create()
{
    _F_;
    const_cast<Mesh *>(this->mesh)->distribute();
    init();
    allocate_objects();
    set_up_matrix_properties();
    set_up_preconditioning();

    set_up_solver_parameters();
    set_up_line_search();
    set_up_monitors();
    set_up_callbacks();

    Problem::create();
}

void
NonlinearProblem::check()
{
    _F_;
    Problem::check();
    if (!validation::in(this->line_search_type, { "bt", "basic", "l2", "cp", "nleqerr", "shell" }))
        log_error("The 'line_search' parameter can be either 'bt', 'basic', 'l2', 'cp', 'nleqerr' "
                  "or 'shell'.");
}

void
NonlinearProblem::init()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(SNESCreate(get_comm(), &this->snes));
    PETSC_CHECK(SNESSetDM(this->snes, dm));
    PETSC_CHECK(DMSetApplicationContext(dm, this));
    PETSC_CHECK(SNESGetKSP(this->snes, &this->ksp));
}

void
NonlinearProblem::set_up_initial_guess()
{
    _F_;
    lprintf(9, "Setting initial guess");
    PETSC_CHECK(VecSet(this->x, 0.));
}

void
NonlinearProblem::allocate_objects()
{
    _F_;
    DM dm = get_dm();
    PETSC_CHECK(DMCreateGlobalVector(dm, &this->x));
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->x, "sln"));

    PETSC_CHECK(VecDuplicate(this->x, &this->r));
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->r, "res"));

    PETSC_CHECK(DMCreateMatrix(dm, &this->J));
    PETSC_CHECK(PetscObjectSetName((PetscObject) this->J, "Jac"));
}

void
NonlinearProblem::set_up_line_search()
{
    _F_;
    SNESLineSearch line_search;
    PETSC_CHECK(SNESGetLineSearch(this->snes, &line_search));
    if (this->line_search_type.compare("basic") == 0)
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHBASIC));
    else if (this->line_search_type.compare("l2") == 0)
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHL2));
    else if (this->line_search_type.compare("cp") == 0)
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHCP));
    else if (this->line_search_type.compare("nleqerr") == 0)
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHNLEQERR));
    else if (this->line_search_type.compare("shell") == 0)
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHSHELL));
    else
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHBT));
    PETSC_CHECK(SNESSetLineSearch(this->snes, line_search));
    PETSC_CHECK(SNESLineSearchSetFromOptions(line_search));
}

void
NonlinearProblem::set_up_callbacks()
{
    _F_;
    PETSC_CHECK(SNESSetFunction(this->snes, this->r, __compute_residual, this));
    PETSC_CHECK(SNESSetJacobian(this->snes, this->J, this->J, __compute_jacobian, this));
}

void
NonlinearProblem::set_up_monitors()
{
    _F_;
    PETSC_CHECK(SNESMonitorSet(this->snes, __snes_monitor, this, 0));
    PETSC_CHECK(KSPMonitorSet(this->ksp, __ksp_monitor, this, 0));
}

void
NonlinearProblem::set_up_solver_parameters()
{
    _F_;
    PETSC_CHECK(SNESSetTolerances(this->snes,
                                  this->nl_abs_tol,
                                  this->nl_rel_tol,
                                  this->nl_step_tol,
                                  this->nl_max_iter,
                                  -1));
    PETSC_CHECK(SNESSetFromOptions(this->snes));

    PETSC_CHECK(KSPSetTolerances(this->ksp,
                                 this->lin_rel_tol,
                                 this->lin_abs_tol,
                                 PETSC_DEFAULT,
                                 this->lin_max_iter));
    PETSC_CHECK(KSPSetFromOptions(this->ksp));
}

PetscErrorCode
NonlinearProblem::snes_monitor_callback(PetscInt it, PetscReal norm)
{
    lprintf(7, "%d Non-linear residual: %e", it, norm);
    return 0;
}

PetscErrorCode
NonlinearProblem::ksp_monitor_callback(PetscInt it, PetscReal rnorm)
{
    lprintf(8, "    %d Linear residual: %e", it, rnorm);
    return 0;
}

void
NonlinearProblem::solve()
{
    _F_;
    lprintf(9, "Solving");
    PETSC_CHECK(SNESSolve(this->snes, nullptr, this->x));
    PETSC_CHECK(SNESGetConvergedReason(this->snes, &this->converged_reason));
}

bool
NonlinearProblem::converged()
{
    _F_;
    bool conv = (this->converged_reason == SNES_CONVERGED_FNORM_ABS) ||
                (this->converged_reason == SNES_CONVERGED_FNORM_RELATIVE) ||
                (this->converged_reason == SNES_CONVERGED_SNORM_RELATIVE) ||
                (this->converged_reason == SNES_CONVERGED_ITS);
    return conv;
}

void
NonlinearProblem::run()
{
    _F_;
    set_up_initial_guess();
    on_initial();
    solve();
    if (converged())
        on_final();
}

void
NonlinearProblem::set_up_matrix_properties()
{
}

void
NonlinearProblem::set_up_preconditioning()
{
}

} // namespace godzilla
