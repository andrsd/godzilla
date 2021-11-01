#include "problems/GPetscLinearProblem.h"
#include "base/CallStack.h"
#include "grids/GGrid.h"
#include "utils/MooseUtils.h"

PetscErrorCode
__compute_rhs(KSP ksp, Vec b, void *ctx)
{
    GPetscLinearProblem * problem = static_cast<GPetscLinearProblem *>(ctx);
    return problem->computeRhsCallback(b);
}

PetscErrorCode
__compute_operators(KSP ksp, Mat A, Mat B, void *ctx)
{
    GPetscLinearProblem * problem = static_cast<GPetscLinearProblem *>(ctx);
    return problem->computeOperatorsCallback(A, B);
}

PetscErrorCode
__ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void *ctx)
{
    GPetscLinearProblem * problem = static_cast<GPetscLinearProblem *>(ctx);
    return problem->kspMonitorCallback(it, rnorm);
}


InputParameters
GPetscLinearProblem::validParams()
{
    InputParameters params = GProblem::validParams();
    params.addPrivateParam<GGrid *>("_ggrid");
    params.addParam<PetscReal>("lin_rel_tol", 1e-5, "Relative convergence tolerance for the linear solver");
    params.addParam<PetscReal>("lin_abs_tol", 1e-50, "Absolute convergence tolerance for the linear solver");
    params.addParam<PetscInt>("lin_max_iter", 10000, "Maximum number of iterations for the linear solver");
    return params;
}

GPetscLinearProblem::GPetscLinearProblem(const InputParameters & parameters) :
    GProblem(parameters),
    grid(*getParam<GGrid *>("_ggrid")),
    lin_rel_tol(getParam<PetscReal>("lin_rel_tol")),
    lin_abs_tol(getParam<PetscReal>("lin_abs_tol")),
    lin_max_iter(getParam<PetscInt>("lin_max_iter"))
{
    _F_;
}

GPetscLinearProblem::~GPetscLinearProblem()
{
    _F_;
    KSPDestroy(&this->ksp);
    VecDestroy(&this->b);
    VecDestroy(&this->x);
    if (this->A != this->B)
        MatDestroy(&this->B);
    MatDestroy(&this->A);
}

const DM &
GPetscLinearProblem::getDM()
{
    return this->grid.getDM();
}

void
GPetscLinearProblem::create()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = KSPCreate(comm().get(), &this->ksp);
    ierr = KSPSetDM(this->ksp, dm);
    ierr = DMSetApplicationContext(dm, this);

    setupProblem();
    allocateObjects();

    setupSolverParameters();
    setupMonitors();
    setupCallbacks();
}

void
GPetscLinearProblem::allocateObjects()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = DMCreateGlobalVector(dm, &this->b);
    ierr = PetscObjectSetName((PetscObject) this->b, "");

    ierr = DMCreateMatrix(dm, &this->A);
    ierr = PetscObjectSetName((PetscObject) this->A, "");
    // TODO: Add API for setting up preconditioners
    this->A = this->B;

    ierr = DMCreateGlobalVector(dm, &this->x);
    ierr = PetscObjectSetName((PetscObject) this->x, "");
}

void
GPetscLinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSetComputeRHS(this->ksp, __compute_rhs, this);
    ierr = KSPSetComputeOperators(this->ksp, __compute_operators, this);
}

void
GPetscLinearProblem::setupMonitors()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPMonitorSet(this->ksp, __ksp_monitor_linear, this, 0);
}

void
GPetscLinearProblem::setupSolverParameters()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSetTolerances(this->ksp,
        this->lin_rel_tol, this->lin_abs_tol, PETSC_DEFAULT,
        this->lin_max_iter);
    ierr = KSPSetFromOptions(ksp);
}

PetscErrorCode
GPetscLinearProblem::computeRhsCallback(Vec b)
{
    _F_;
    return 0;
}

PetscErrorCode
GPetscLinearProblem::computeOperatorsCallback(Mat A, Mat B)
{
    _F_;
    return 0;
}

PetscErrorCode
GPetscLinearProblem::kspMonitorCallback(PetscInt it, PetscReal rnorm)
{
    _F_;
    godzillaPrint(8, it, " Linear residual: ", std::scientific, rnorm);
    return 0;
}

void
GPetscLinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSolve(this->ksp, this->b, this->x);
    ierr = KSPGetConvergedReason(this->ksp, &this->converged_reason);
}

bool
GPetscLinearProblem::converged()
{
    _F_;
    bool conv =
        this->converged_reason == KSP_CONVERGED_RTOL_NORMAL ||
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
GPetscLinearProblem::out()
{
    _F_;
}
