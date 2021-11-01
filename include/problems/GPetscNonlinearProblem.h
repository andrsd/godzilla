#pragma once

#include "problems/GProblem.h"
#include "petscsnes.h"

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
    virtual const DM & getDM() = 0;
    /// setup the problem to be solved
    virtual void setupProblem() = 0;
    /// setup initial guess
    virtual void setInitialGuess() = 0;
    /// setup line search
    virtual void setupLineSearch();
    /// Setup computation of Jacobian
    virtual void setupJacobian();
    /// Setup monitors
    virtual void setupMonitors();
    /// output
    virtual void out() override;
    /// Method to compute Jacobian. Called from the PETsc callback
    PetscErrorCode computeJacobianCallback(Mat jac, Vec x);
    /// SNES monitor
    PetscErrorCode snesMonitorCallback(PetscInt it, PetscReal norm);
    /// KSP monitor
    PetscErrorCode kspMonitorCallback(PetscInt it, PetscReal rnorm);

    /// SNES objects
    SNES snes;
    /// The solution vector
    Vec x;
    /// Jacobian matrix
    Mat J;
    /// Preconditioning matrix
    Mat A;
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

    friend PetscErrorCode __compute_jacobian(SNES snes, Vec x, Mat jac, Mat B, void *ctx);
    friend PetscErrorCode __ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void *ctx);
    friend PetscErrorCode __snes_monitor(SNES snes, PetscInt it, PetscReal norm, void *ctx);
};
