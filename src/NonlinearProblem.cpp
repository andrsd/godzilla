#include "NonlinearProblem.h"
#include "CallStack.h"
#include "Utils.h"
#include "Grid.h"
#include "Output.h"
#include "petscds.h"

namespace godzilla {

PetscErrorCode
__compute_residual(SNES snes, Vec x, Vec f, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->computeResidualCallback(x, f);
}

PetscErrorCode
__compute_jacobian(SNES snes, Vec x, Mat J, Mat Jp, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->computeJacobianCallback(x, J, Jp);
}

PetscErrorCode
__ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->kspMonitorCallback(it, rnorm);
}

PetscErrorCode
__snes_monitor(SNES snes, PetscInt it, PetscReal norm, void * ctx)
{
    _F_;
    NonlinearProblem * problem = static_cast<NonlinearProblem *>(ctx);
    return problem->snesMonitorCallback(it, norm);
}

InputParameters
NonlinearProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addParam<std::string>("line_search", "bt", "The type of line search to be used");
    params.addParam<PetscReal>("nl_rel_tol",
                               1e-8,
                               "Relative convergence tolerance for the non-linear solver");
    params.addParam<PetscReal>("nl_abs_tol",
                               1e-15,
                               "Absolute convergence tolerance for the non-linear solver");
    params.addParam<PetscReal>(
        "nl_step_tol",
        1e-15,
        "Convergence tolerance in terms of the norm of the change in the solution between steps");
    params.addParam<PetscInt>("nl_max_iter",
                              40,
                              "Maximum number of iterations for the non-linear solver");
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

NonlinearProblem::NonlinearProblem(const InputParameters & parameters) :
    Problem(parameters),
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

const DM &
NonlinearProblem::getDM() const
{
    _F_;
    return this->grid.getDM();
}

const Vec &
NonlinearProblem::getSolutionVector() const
{
    _F_;
    return this->x;
}

void
NonlinearProblem::create()
{
    _F_;
    init();
    allocateObjects();
    onSetMatrixProperties();

    setupSolverParameters();
    setupLineSearch();
    setupMonitors();
    setupCallbacks();

    setupInitialGuess();
}

void
NonlinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = SNESCreate(comm(), &this->snes);
    checkPetscError(ierr);
    ierr = SNESSetDM(this->snes, dm);
    checkPetscError(ierr);
    ierr = DMSetApplicationContext(dm, this);
    checkPetscError(ierr);
}

void
NonlinearProblem::setupInitialGuess()
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSet(this->x, 0.);
    checkPetscError(ierr);
}

void
NonlinearProblem::allocateObjects()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = getDM();

    ierr = DMCreateGlobalVector(dm, &this->x);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->x, "sln");
    checkPetscError(ierr);

    ierr = VecDuplicate(this->x, &this->r);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->r, "res");
    checkPetscError(ierr);

    ierr = DMCreateMatrix(dm, &this->J);
    checkPetscError(ierr);
    ierr = PetscObjectSetName((PetscObject) this->J, "Jac");
    checkPetscError(ierr);

    // full newton
    this->Jp = this->J;
}

void
NonlinearProblem::setupLineSearch()
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
NonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetFunction(this->snes, this->r, __compute_residual, this);
    checkPetscError(ierr);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, __compute_jacobian, this);
    checkPetscError(ierr);
}

void
NonlinearProblem::setupMonitors()
{
    _F_;
    SNESMonitorSet(this->snes, __snes_monitor, this, 0);

    KSP ksp;
    SNESGetKSP(this->snes, &ksp);
    KSPMonitorSet(ksp, __ksp_monitor, this, 0);
}

void
NonlinearProblem::setupSolverParameters()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSetTolerances(this->snes,
                             this->nl_abs_tol,
                             this->nl_rel_tol,
                             this->nl_step_tol,
                             this->nl_max_iter,
                             -1);
    checkPetscError(ierr);
    ierr = SNESSetFromOptions(this->snes);
    checkPetscError(ierr);

    KSP ksp;
    ierr = SNESGetKSP(this->snes, &ksp);
    checkPetscError(ierr);
    ierr = KSPSetTolerances(ksp,
                            this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
    checkPetscError(ierr);
    ierr = KSPSetFromOptions(ksp);
    checkPetscError(ierr);
}

PetscErrorCode
NonlinearProblem::snesMonitorCallback(PetscInt it, PetscReal norm)
{
    godzillaPrint(7, it, " Non-linear residual: ", std::scientific, norm);
    return 0;
}

PetscErrorCode
NonlinearProblem::kspMonitorCallback(PetscInt it, PetscReal rnorm)
{
    godzillaPrint(8, "    ", it, " Linear residual: ", std::scientific, rnorm);
    return 0;
}

void
NonlinearProblem::solve()
{
    _F_;
    PetscErrorCode ierr;
    ierr = SNESSolve(this->snes, NULL, this->x);
    checkPetscError(ierr);
    ierr = SNESGetConvergedReason(this->snes, &this->converged_reason);
    checkPetscError(ierr);
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
    solve();
    if (converged())
        output();
}

void
NonlinearProblem::output()
{
    _F_;
    for (auto & o : this->outputs) {
        o->setFileName();
        o->output(getDM(), this->x);
    }
}

void
NonlinearProblem::onSetMatrixProperties()
{
}

} // namespace godzilla
