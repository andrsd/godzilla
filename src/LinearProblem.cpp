#include "LinearProblem.h"
#include "CallStack.h"
#include "Grid.h"
#include "Utils.h"

namespace godzilla {

PetscErrorCode
__compute_rhs(KSP ksp, Vec b, void * ctx)
{
    _F_;
    LinearProblem * problem = static_cast<LinearProblem *>(ctx);
    return problem->computeRhsCallback(b);
}

PetscErrorCode
__compute_operators(KSP ksp, Mat A, Mat B, void * ctx)
{
    _F_;
    LinearProblem * problem = static_cast<LinearProblem *>(ctx);
    return problem->computeOperatorsCallback(A, B);
}

PetscErrorCode
__ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    LinearProblem * problem = static_cast<LinearProblem *>(ctx);
    return problem->kspMonitorCallback(it, rnorm);
}

InputParameters
LinearProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addParam<PetscReal>("lin_rel_tol",
                               1e-5,
                               "Relative convergence tolerance for the linear solver");
    params.addParam<PetscReal>("lin_abs_tol",
                               1e-50,
                               "Absolute convergence tolerance for the linear solver");
    params.addParam<PetscInt>("lin_max_iter",
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
    lin_rel_tol(getParam<PetscReal>("lin_rel_tol")),
    lin_abs_tol(getParam<PetscReal>("lin_abs_tol")),
    lin_max_iter(getParam<PetscInt>("lin_max_iter"))
{
    _F_;
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

const DM &
LinearProblem::getDM() const
{
    _F_;
    return this->grid.getDM();
}

const Vec &
LinearProblem::getSolutionVector() const
{
    _F_;
    return this->x;
}

void
LinearProblem::create()
{
    _F_;
    init();
    allocateObjects();

    setupSolverParameters();
    setupMonitors();
    setupCallbacks();
}

void
LinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = KSPCreate(comm(), &this->ksp);
    checkPetscError(ierr);
    ierr = KSPSetDM(this->ksp, dm);
    checkPetscError(ierr);
    ierr = DMSetApplicationContext(dm, this);
    checkPetscError(ierr);
}

void
LinearProblem::allocateObjects()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = DMCreateGlobalVector(dm, &this->x);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "");
    checkPetscError(ierr);

    ierr = VecDuplicate(this->x, &this->b);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->b, "");
    checkPetscError(ierr);

    ierr = DMCreateMatrix(dm, &this->A);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->A, "");
    checkPetscError(ierr);
    // TODO: Add API for setting up preconditioners
    this->A = this->B;
}

void
LinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSetComputeRHS(this->ksp, __compute_rhs, this);
    checkPetscError(ierr);
    ierr = KSPSetComputeOperators(this->ksp, __compute_operators, this);
    checkPetscError(ierr);
}

void
LinearProblem::setupMonitors()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPMonitorSet(this->ksp, __ksp_monitor_linear, this, 0);
    checkPetscError(ierr);
}

void
LinearProblem::setupSolverParameters()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSetTolerances(this->ksp,
                            this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
    checkPetscError(ierr);
    ierr = KSPSetFromOptions(ksp);
    checkPetscError(ierr);
}

PetscErrorCode
LinearProblem::kspMonitorCallback(PetscInt it, PetscReal rnorm)
{
    _F_;
    godzillaPrint(8, it, " Linear residual: ", std::scientific, rnorm);
    return 0;
}

void
LinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;

    ierr = KSPSolve(this->ksp, this->b, this->x);
    checkPetscError(ierr);
    ierr = KSPGetConvergedReason(this->ksp, &this->converged_reason);
    checkPetscError(ierr);
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

} // namespace godzilla
