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
    /// Setup computation of Jacobian
    virtual void setupJacobian();
    /// output
    virtual void out() override;
    /// Method to compute Jacobian. Called from the PETsc callback
    PetscErrorCode computeJacobianCallback(Mat jac, Vec x);

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

public:
    static InputParameters validParams();

    friend PetscErrorCode __compute_jacobian(SNES snes, Vec x, Mat jac, Mat B, void *ctx);
};
