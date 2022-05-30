#include "LinearProblem.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Utils.h"
#include "Output.h"

namespace godzilla {

PetscErrorCode
__compute_rhs(KSP ksp, Vec b, void * ctx)
{
    _F_;
    LinearProblem * problem = static_cast<LinearProblem *>(ctx);
    return problem->compute_rhs_callback(b);
}

PetscErrorCode
__compute_operators(KSP ksp, Mat A, Mat B, void * ctx)
{
    _F_;
    LinearProblem * problem = static_cast<LinearProblem *>(ctx);
    return problem->compute_operators_callback(A, B);
}

PetscErrorCode
__ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    LinearProblem * problem = static_cast<LinearProblem *>(ctx);
    return problem->ksp_monitor_callback(it, rnorm);
}

InputParameters
LinearProblem::valid_params()
{
    InputParameters params = Problem::valid_params();
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

LinearProblem::LinearProblem(const InputParameters & parameters) :
    Problem(parameters),
    ksp(NULL),
    x(NULL),
    b(NULL),
    A(NULL),
    B(NULL),
    lin_rel_tol(get_param<PetscReal>("lin_rel_tol")),
    lin_abs_tol(get_param<PetscReal>("lin_abs_tol")),
    lin_max_iter(get_param<PetscInt>("lin_max_iter"))
{
    _F_;
    this->default_output_on = Output::ON_FINAL;
}

LinearProblem::~LinearProblem()
{
    _F_;
    if (this->ksp)
        KSPDestroy(&this->ksp);
    if (this->b)
        VecDestroy(&this->b);
    if (this->x)
        VecDestroy(&this->x);
    if ((this->A != this->B) && (this->B))
        MatDestroy(&this->B);
    if (this->A)
        MatDestroy(&this->A);
}

DM
LinearProblem::get_dm() const
{
    _F_;
    return this->mesh->get_dm();
}

Vec
LinearProblem::get_solution_vector() const
{
    _F_;
    return this->x;
}

void
LinearProblem::create()
{
    _F_;
    init();
    allocate_objects();
    set_up_matrix_properties();

    set_up_solver_parameters();
    set_up_monitors();
    set_up_callbacks();

    Problem::create();
}

void
LinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = KSPCreate(get_comm(), &this->ksp);
    check_petsc_error(ierr);
    ierr = KSPSetDM(this->ksp, dm);
    check_petsc_error(ierr);
    ierr = DMSetApplicationContext(dm, this);
    check_petsc_error(ierr);
}

void
LinearProblem::allocate_objects()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = get_dm();

    ierr = DMCreateGlobalVector(dm, &this->x);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "sln");
    check_petsc_error(ierr);

    ierr = VecDuplicate(this->x, &this->b);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) this->b, "rhs");
    check_petsc_error(ierr);

    ierr = DMCreateMatrix(dm, &this->A);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) this->A, "A");
    check_petsc_error(ierr);
    // TODO: Add API for setting up preconditioners
    this->B = this->A;
}

void
LinearProblem::set_up_callbacks()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSetComputeRHS(this->ksp, __compute_rhs, this);
    check_petsc_error(ierr);
    ierr = KSPSetComputeOperators(this->ksp, __compute_operators, this);
    check_petsc_error(ierr);
}

void
LinearProblem::set_up_monitors()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPMonitorSet(this->ksp, __ksp_monitor_linear, this, 0);
    check_petsc_error(ierr);
}

void
LinearProblem::set_up_solver_parameters()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSetTolerances(this->ksp,
                            this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
    check_petsc_error(ierr);
    ierr = KSPSetFromOptions(ksp);
    check_petsc_error(ierr);
}

PetscErrorCode
LinearProblem::ksp_monitor_callback(PetscInt it, PetscReal rnorm)
{
    _F_;
    lprintf(8, "%d Linear residual: %e", it, rnorm);
    return 0;
}

void
LinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSolve(this->ksp, this->b, this->x);
    check_petsc_error(ierr);
    ierr = KSPGetConvergedReason(this->ksp, &this->converged_reason);
    check_petsc_error(ierr);
}

bool
LinearProblem::converged()
{
    _F_;
    bool conv = this->converged_reason == KSP_CONVERGED_RTOL_NORMAL ||
                this->converged_reason == KSP_CONVERGED_ATOL_NORMAL ||
                this->converged_reason == KSP_CONVERGED_RTOL ||
                this->converged_reason == KSP_CONVERGED_ATOL ||
                this->converged_reason == KSP_CONVERGED_ITS ||
                this->converged_reason == KSP_CONVERGED_CG_NEG_CURVE ||
                this->converged_reason == KSP_CONVERGED_CG_CONSTRAINED ||
                this->converged_reason == KSP_CONVERGED_STEP_LENGTH ||
                this->converged_reason == KSP_CONVERGED_HAPPY_BREAKDOWN;
    return conv;
}

void
LinearProblem::run()
{
    _F_;
    solve();
    compute_postprocessors();
    if (converged())
        output_final();
}

void
LinearProblem::set_up_matrix_properties()
{
}

} // namespace godzilla
