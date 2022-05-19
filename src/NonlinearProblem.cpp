#include "NonlinearProblem.h"
#include "CallStack.h"
#include "Utils.h"
#include "Mesh.h"
#include "Output.h"
#include "petscds.h"

namespace godzilla {

PetscErrorCode
__compute_residual(SNES snes, Vec x, Vec f, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->compute_residual_callback(x, f);
}

PetscErrorCode
__compute_jacobian(SNES snes, Vec x, Mat J, Mat Jp, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->compute_jacobian_callback(x, J, Jp);
}

PetscErrorCode
__ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

PetscErrorCode
__snes_monitor(SNES snes, PetscInt it, PetscReal norm, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->snes_monitor_callback(it, norm);
}

InputParameters
NonlinearProblem::valid_params()
{
    InputParameters params = Problem::valid_params();
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

NonlinearProblem::NonlinearProblem(const InputParameters & parameters) :
    Problem(parameters),
    snes(NULL),
    x(NULL),
    r(NULL),
    J(NULL),
    Jp(NULL),
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
    if ((this->Jp != this->J) && (this->Jp))
        MatDestroy(&this->Jp);
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
    init();
    allocate_objects();
    on_set_matrix_properties();

    set_up_solver_parameters();
    set_up_line_search();
    set_up_monitors();
    set_up_callbacks();

    Problem::create();
}

void
NonlinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = SNESCreate(get_comm(), &this->snes);
    check_petsc_error(ierr);
    ierr = SNESSetDM(this->snes, dm);
    check_petsc_error(ierr);
    ierr = DMSetApplicationContext(dm, this);
    check_petsc_error(ierr);
}

void
NonlinearProblem::set_up_initial_guess()
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSet(this->x, 0.);
    check_petsc_error(ierr);
}

void
NonlinearProblem::allocate_objects()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = DMCreateGlobalVector(dm, &this->x);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "sln");
    check_petsc_error(ierr);

    ierr = VecDuplicate(this->x, &this->r);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) this->r, "res");
    check_petsc_error(ierr);

    ierr = DMCreateMatrix(dm, &this->J);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) this->J, "Jac");
    check_petsc_error(ierr);

    // full newton
    this->Jp = this->J;
}

void
NonlinearProblem::set_up_line_search()
{
    _F_;
    SNESLineSearch line_search;
    SNESGetLineSearch(this->snes, &line_search);
    if (this->line_search_type.compare("basic") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHBASIC);
    else if (this->line_search_type.compare("l2") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHL2);
    else if (this->line_search_type.compare("cp") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHCP);
    else if (this->line_search_type.compare("nleqerr") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHNLEQERR);
    else if (this->line_search_type.compare("shell") == 0)
        SNESLineSearchSetType(line_search, SNESLINESEARCHSHELL);
    else
        SNESLineSearchSetType(line_search, SNESLINESEARCHBT);
    SNESSetLineSearch(this->snes, line_search);
    SNESLineSearchSetFromOptions(line_search);
}

void
NonlinearProblem::set_up_callbacks()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetFunction(this->snes, this->r, __compute_residual, this);
    check_petsc_error(ierr);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, __compute_jacobian, this);
    check_petsc_error(ierr);
}

void
NonlinearProblem::set_up_monitors()
{
    _F_;
    SNESMonitorSet(this->snes, __snes_monitor, this, 0);

    KSP ksp;
    SNESGetKSP(this->snes, &ksp);
    KSPMonitorSet(ksp, __ksp_monitor, this, 0);
}

void
NonlinearProblem::set_up_solver_parameters()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetTolerances(this->snes,
                             this->nl_abs_tol,
                             this->nl_rel_tol,
                             this->nl_step_tol,
                             this->nl_max_iter,
                             -1);
    check_petsc_error(ierr);
    ierr = SNESSetFromOptions(this->snes);
    check_petsc_error(ierr);

    KSP ksp;
    ierr = SNESGetKSP(this->snes, &ksp);
    check_petsc_error(ierr);
    ierr = KSPSetTolerances(ksp,
                            this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
    check_petsc_error(ierr);
    ierr = KSPSetFromOptions(ksp);
    check_petsc_error(ierr);
}

PetscErrorCode
NonlinearProblem::snes_monitor_callback(PetscInt it, PetscReal norm)
{
    godzilla_print(7, "%d Non-linear residual: %e", it, norm);
    return 0;
}

PetscErrorCode
NonlinearProblem::ksp_monitor_callback(PetscInt it, PetscReal rnorm)
{
    godzilla_print(8, "    %d Linear residual: %e", it, rnorm);
    return 0;
}

void
NonlinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSolve(this->snes, NULL, this->x);
    check_petsc_error(ierr);
    ierr = SNESGetConvergedReason(this->snes, &this->converged_reason);
    check_petsc_error(ierr);
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
    solve();
    compute_postprocessors();
    if (converged())
        output();
}

void
NonlinearProblem::output()
{
    _F_;
    for (auto & o : this->outputs)
        o->output_step(-1);
}

void
NonlinearProblem::on_set_matrix_properties()
{
}

} // namespace godzilla
