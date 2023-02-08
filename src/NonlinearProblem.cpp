#include "NonlinearProblem.h"
#include "CallStack.h"
#include "Utils.h"
#include "Mesh.h"
#include "Output.h"
#include "Validation.h"
#include "petscds.h"

namespace godzilla {

static PetscErrorCode
__compute_residual(SNES, Vec x, Vec f, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    Vector vec_x(x);
    Vector vec_f(f);
    return problem->compute_residual(vec_x, vec_f);
}

static PetscErrorCode
__compute_jacobian(SNES, Vec x, Mat J, Mat Jp, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    Vector vec_x(x);
    Matrix mat_J(J);
    Matrix mat_Jp(Jp);
    return problem->compute_jacobian(vec_x, mat_J, mat_Jp);
}

PetscErrorCode
__ksp_monitor(KSP, Int it, Real rnorm, void * ctx)
{
    _F_;
    auto * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

PetscErrorCode
__snes_monitor(SNES, Int it, Real norm, void * ctx)
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
    params.add_param<Real>("nl_rel_tol",
                           1e-8,
                           "Relative convergence tolerance for the non-linear solver");
    params.add_param<Real>("nl_abs_tol",
                           1e-15,
                           "Absolute convergence tolerance for the non-linear solver");
    params.add_param<Real>(
        "nl_step_tol",
        1e-15,
        "Convergence tolerance in terms of the norm of the change in the solution between steps");
    params.add_param<Int>("nl_max_iter",
                          40,
                          "Maximum number of iterations for the non-linear solver");
    params.add_param<Real>("lin_rel_tol",
                           1e-5,
                           "Relative convergence tolerance for the linear solver");
    params.add_param<Real>("lin_abs_tol",
                           1e-50,
                           "Absolute convergence tolerance for the linear solver");
    params.add_param<Int>("lin_max_iter",
                          10000,
                          "Maximum number of iterations for the linear solver");
    return params;
}

NonlinearProblem::NonlinearProblem(const Parameters & parameters) :
    Problem(parameters),
    snes(nullptr),
    ksp(nullptr),
    converged_reason(SNES_CONVERGED_ITERATING),
    line_search_type(get_param<std::string>("line_search")),
    nl_rel_tol(get_param<Real>("nl_rel_tol")),
    nl_abs_tol(get_param<Real>("nl_abs_tol")),
    nl_step_tol(get_param<Real>("nl_step_tol")),
    nl_max_iter(get_param<Int>("nl_max_iter")),
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter"))
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
    this->r.destroy();
    this->x.destroy();
    this->J.destroy();
}

DM
NonlinearProblem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

const Vector &
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
    this->x.set(0.);
}

void
NonlinearProblem::allocate_objects()
{
    _F_;
    DM dm = get_dm();
    Vec glob_x;
    PETSC_CHECK(DMCreateGlobalVector(dm, &glob_x));
    this->x = Vector(glob_x);
    this->x.set_name("sln");

    this->x.duplicate(this->r);
    this->r.set_name("res");

    Mat mat_J;
    PETSC_CHECK(DMCreateMatrix(dm, &mat_J));
    this->J = Matrix(mat_J);
    this->J.set_name("Jac");
}

void
NonlinearProblem::set_up_line_search()
{
    _F_;
    SNESLineSearch line_search;
    PETSC_CHECK(SNESGetLineSearch(this->snes, &line_search));
    if (this->line_search_type == "basic")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHBASIC));
    else if (this->line_search_type == "l2")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHL2));
    else if (this->line_search_type == "cp")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHCP));
    else if (this->line_search_type == "nleqerr")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHNLEQERR));
    else if (this->line_search_type == "shell")
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
    PETSC_CHECK(SNESSetFunction(this->snes, (Vec) this->r, __compute_residual, this));
    PETSC_CHECK(
        SNESSetJacobian(this->snes, (Mat) this->J, (Mat) this->J, __compute_jacobian, this));
}

void
NonlinearProblem::set_up_monitors()
{
    _F_;
    PETSC_CHECK(SNESMonitorSet(this->snes, __snes_monitor, this, nullptr));
    PETSC_CHECK(KSPMonitorSet(this->ksp, __ksp_monitor, this, nullptr));
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
NonlinearProblem::snes_monitor_callback(Int it, Real norm)
{
    lprintf(7, "%d Non-linear residual: %e", it, norm);
    return 0;
}

PetscErrorCode
NonlinearProblem::ksp_monitor_callback(Int it, Real rnorm)
{
    lprintf(8, "    %d Linear residual: %e", it, rnorm);
    return 0;
}

void
NonlinearProblem::solve()
{
    _F_;
    lprintf(9, "Solving");
    PETSC_CHECK(SNESSolve(this->snes, nullptr, (Vec) this->x));
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

PetscErrorCode
NonlinearProblem::compute_residual(const Vector &, Vector &)
{
    _F_;
    return 0;
}

PetscErrorCode
NonlinearProblem::compute_jacobian(const Vector &, Matrix &, Matrix &)
{
    _F_;
    return 0;
}

} // namespace godzilla
