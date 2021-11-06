#include "problems/GNonlinearProblem.h"
#include "base/CallStack.h"
#include "utils/Utils.h"
#include "grids/GGrid.h"
#include "petscds.h"


namespace godzilla {

PetscErrorCode
__compute_residual(SNES snes, Vec x, Vec f, void *ctx)
{
    GNonlinearProblem * problem = static_cast<GNonlinearProblem *>(ctx);
    return problem->computeResidualCallback(x, f);
}

PetscErrorCode
__compute_jacobian(SNES snes, Vec x, Mat J, Mat Jp, void *ctx)
{
    GNonlinearProblem * problem = static_cast<GNonlinearProblem *>(ctx);
    return problem->computeJacobianCallback(x, J, Jp);
}

PetscErrorCode
__ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void *ctx)
{
    GNonlinearProblem * problem = static_cast<GNonlinearProblem *>(ctx);
    return problem->kspMonitorCallback(it, rnorm);
}

PetscErrorCode
__snes_monitor(SNES snes, PetscInt it, PetscReal norm, void *ctx)
{
    GNonlinearProblem * problem = static_cast<GNonlinearProblem *>(ctx);
    return problem->snesMonitorCallback(it, norm);
}


InputParameters
GNonlinearProblem::validParams()
{
    InputParameters params = GProblem::validParams();
    params.addParam<std::string>("line_search", "bt", "The type of line search to be used");
    params.addParam<PetscReal>("nl_rel_tol", 1e-8, "Relative convergence tolerance for the non-linear solver");
    params.addParam<PetscReal>("nl_abs_tol", 1e-15, "Absolute convergence tolerance for the non-linear solver");
    params.addParam<PetscReal>("nl_step_tol", 1e-15, "Convergence tolerance in terms of the norm of the change in the solution between steps");
    params.addParam<PetscInt>("nl_max_iter", 40, "Maximum number of iterations for the non-linear solver");
    params.addParam<PetscReal>("lin_rel_tol", 1e-5, "Relative convergence tolerance for the linear solver");
    params.addParam<PetscReal>("lin_abs_tol", 1e-50, "Absolute convergence tolerance for the linear solver");
    params.addParam<PetscInt>("lin_max_iter", 10000, "Maximum number of iterations for the linear solver");
    return params;
}

GNonlinearProblem::GNonlinearProblem(const InputParameters & parameters) :
    GProblem(parameters),
    snes(NULL),
    x(NULL),
    r(NULL),
    J(NULL),
    Jp(NULL),
    line_search_type(getParam<std::string>("line_search")),
    nl_rel_tol(getParam<PetscReal>("nl_rel_tol")),
    nl_abs_tol(getParam<PetscReal>("nl_abs_tol")),
    nl_step_tol(getParam<PetscReal>("nl_step_tol")),
    nl_max_iter(getParam<PetscInt>("nl_max_iter")),
    lin_rel_tol(getParam<PetscReal>("lin_rel_tol")),
    lin_abs_tol(getParam<PetscReal>("lin_abs_tol")),
    lin_max_iter(getParam<PetscInt>("lin_max_iter"))
{
    _F_;
    line_search_type = utils::toLower(line_search_type);
}

GNonlinearProblem::~GNonlinearProblem()
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

const DM &
GNonlinearProblem::getDM() const
{
    return this->grid.getDM();
}

const Vec &
GNonlinearProblem::getSolutionVector() const
{
    return this->x;
}

void
GNonlinearProblem::create()
{
    _F_;
    init();
    allocateObjects();

    setupSolverParameters();
    setupLineSearch();
    setupMonitors();
    setupCallbacks();

    setupInitialGuess();
}

void
GNonlinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = SNESCreate(comm(), &this->snes);
    ierr = SNESSetDM(this->snes, dm);
    ierr = DMSetApplicationContext(dm, this);
}

void
GNonlinearProblem::setupInitialGuess()
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSet(this->x, 0.);
}

void
GNonlinearProblem::allocateObjects()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = DMCreateGlobalVector(dm, &this->x);
    ierr = PetscObjectSetName((PetscObject) this->x, "");

    ierr = VecDuplicate(this->x, &this->r);
    ierr = PetscObjectSetName((PetscObject) this->r, "");

    ierr = DMCreateMatrix(dm, &this->J);
    ierr = PetscObjectSetName((PetscObject) this->J, "");

    // full newton
    this->Jp = this->J;
}

void
GNonlinearProblem::setupLineSearch()
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
GNonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetFunction(this->snes, this->r, __compute_residual, this);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, __compute_jacobian, this);
}

void
GNonlinearProblem::setupMonitors()
{
    _F_;
    SNESMonitorSet(this->snes, __snes_monitor, this, 0);

    KSP ksp;
    SNESGetKSP(this->snes, &ksp);
    KSPMonitorSet(ksp, __ksp_monitor, this, 0);
}

void
GNonlinearProblem::setupSolverParameters()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetTolerances(this->snes,
        this->nl_abs_tol, this->nl_rel_tol, this->nl_step_tol,
        this->nl_max_iter, -1);
    ierr = SNESSetFromOptions(this->snes);

    KSP ksp;
    ierr = SNESGetKSP(this->snes, &ksp);
    ierr = KSPSetTolerances(ksp,
        this->lin_rel_tol, this->lin_abs_tol, PETSC_DEFAULT,
        this->lin_max_iter);
    ierr = KSPSetFromOptions(ksp);
}

PetscErrorCode
GNonlinearProblem::snesMonitorCallback(PetscInt it, PetscReal norm)
{
    godzillaPrint(7, it, " Non-linear residual: ", std::scientific, norm);
    return 0;
}

PetscErrorCode
GNonlinearProblem::kspMonitorCallback(PetscInt it, PetscReal rnorm)
{
    godzillaPrint(8, "    ", it, " Linear residual: ", std::scientific, rnorm);
    return 0;
}

void
GNonlinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSolve(this->snes, NULL, this->x);
    ierr = SNESGetConvergedReason(this->snes, &this->converged_reason);
}

bool
GNonlinearProblem::converged()
{
    _F_;
    bool conv =
        (this->converged_reason == SNES_CONVERGED_FNORM_ABS) ||
        (this->converged_reason == SNES_CONVERGED_FNORM_RELATIVE) ||
        (this->converged_reason == SNES_CONVERGED_SNORM_RELATIVE) ||
        (this->converged_reason == SNES_CONVERGED_ITS);
    return conv;
}

}
