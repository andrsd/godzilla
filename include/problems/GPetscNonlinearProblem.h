#pragma once

#include "problems/GProblem.h"
#include "petscsnes.h"

namespace godzilla {

class GGrid;

/// PETSc non-linear problem
///
class GPetscNonlinearProblem : public GProblem
{
public:
    GPetscNonlinearProblem(const InputParameters & parameters);
    virtual ~GPetscNonlinearProblem();

    virtual void create() override;
    virtual void solve() override;
    virtual bool converged() override;

protected:
    /// provide DM for the underlying SNES object
    virtual const DM & getDM();
    /// setup the problem to be solved
    virtual void setupProblem() = 0;
    /// setup initial guess
    virtual void setupInitialGuess();
    /// Allocate Jacobian/residual objects
    virtual void allocateObjects();
    /// setup line search
    virtual void setupLineSearch();
    /// Setup computation of residual and Jacobian callbacks
    virtual void setupCallbacks();
    /// Setup monitors
    virtual void setupMonitors();
    /// Setup solver parameters
    virtual void setupSolverParameters();
    /// output
    virtual void out() override;
    /// Method to compute residual. Called from the PETsc callback
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) = 0;
    /// Method to compute Jacobian. Called from the PETsc callback
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) = 0;
    /// SNES monitor
    PetscErrorCode snesMonitorCallback(PetscInt it, PetscReal norm);
    /// KSP monitor
    PetscErrorCode kspMonitorCallback(PetscInt it, PetscReal rnorm);

    /// Grid
    GGrid & grid;
    /// SNES object
    SNES snes;
    /// The solution vector
    Vec x;
    /// The residual vector
    Vec r;
    /// Jacobian matrix
    Mat J;
    /// Preconditioning matrix
    Mat Jp;
    /// Converged reason
    SNESConvergedReason converged_reason;

    /// The type of line search to be used
    std::string line_search_type;
    /// Relative convergence tolerance for the non-linear solver
    PetscReal nl_rel_tol;
    /// Absolute convergence tolerance for the non-linear solver
    PetscReal nl_abs_tol;
    /// Convergence tolerance in terms of the norm of the change in the solution between steps
    PetscReal nl_step_tol;
    /// Maximum number of iterations for the non-linear solver
    PetscInt nl_max_iter;
    /// Relative convergence tolerance for the linear solver
    PetscReal lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    PetscReal lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    PetscInt lin_max_iter;

public:
    static InputParameters validParams();

    friend PetscErrorCode __compute_residual(SNES snes, Vec x, Vec f, void *ctx);
    friend PetscErrorCode __compute_jacobian(SNES snes, Vec x, Mat A, Mat B, void *ctx);
    friend PetscErrorCode __ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void *ctx);
    friend PetscErrorCode __snes_monitor(SNES snes, PetscInt it, PetscReal norm, void *ctx);
};

}
