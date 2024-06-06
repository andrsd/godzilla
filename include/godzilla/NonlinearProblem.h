// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/SNESolver.h"

namespace godzilla {

/// PETSc non-linear problem
///
class NonlinearProblem : public Problem {
public:
    explicit NonlinearProblem(const Parameters & parameters);
    ~NonlinearProblem() override;

    void create() override;
    void run() override;

    /// Get underlying KSP
    KrylovSolver get_ksp() const;
    /// Set KSP operators
    void set_ksp_operators(const Matrix & A, const Matrix & B);
    /// Get Jacobian matrix
    const Matrix & get_jacobian() const;
    /// true if solve converged, otherwise false
    virtual bool converged();

    /// Use matrix free finite difference matrix vector products to apply the Jacobian
    ///
    /// @param mf_operator use matrix-free only the A matrix
    /// @param mf use matrix-free for both the A and P matrices
    void set_use_matrix_free(bool mf_operator, bool mf);

protected:
    /// Get underlying non-linear solver
    SNESolver get_snes() const;
    /// Set non-linear solver
    void set_snes(const SNESolver & snes);
    /// Set residual vector
    void set_residual_vector(const Vector & f);
    /// Set Jacobian matrix
    void set_jacobian_matrix(const Matrix & J);
    /// Initialize the problem
    virtual void init();
    /// Set up initial guess
    virtual void set_up_initial_guess();
    /// Allocate Jacobian/residual objects
    void allocate_objects() override;
    /// Set up line search
    virtual void set_up_line_search();
    /// Set up computation of residual and Jacobian callbacks
    virtual void set_up_callbacks();
    /// Set up monitors
    virtual void set_up_monitors();
    /// Set up solve type
    virtual void set_up_solve_type();
    /// Set up solver parameters
    virtual void set_up_solver_parameters();
    /// Set the function evaluation routine
    template <class T>
    void
    set_function(T * instance, ErrorCode (T::*callback)(const Vector &, Vector &))
    {
        this->snes.set_function(this->r, instance, callback);
    }
    /// Set the function to compute Jacobian
    template <class T>
    void
    set_jacobian(T * instance, ErrorCode (T::*callback)(const Vector &, Matrix &, Matrix &))
    {
        this->snes.set_jacobian(this->J, this->J, instance, callback);
    }
    /// SNES monitor
    ErrorCode snes_monitor(Int it, Real norm);
    /// KSP monitor
    ErrorCode ksp_monitor(Int it, Real rnorm);
    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();
    /// Method for creating a preconditioner
    virtual Preconditioner create_preconditioner(PC pc);
    /// Solve the problem
    virtual void solve();

private:
    void set_up_preconditioning();

    /// Nonlinear solver
    SNESolver snes;
    /// Linear solver
    KrylovSolver ksp;
    /// The residual vector
    Vector r;
    /// Jacobian matrix
    Matrix J;
    /// Converged reason
    SNESolver::ConvergedReason converged_reason;
    /// Preconditioner
    Preconditioner precond;

    /// The type of line search to be used
    std::string line_search_type;
    /// Relative convergence tolerance for the non-linear solver
    Real nl_rel_tol;
    /// Absolute convergence tolerance for the non-linear solver
    Real nl_abs_tol;
    /// Convergence tolerance in terms of the norm of the change in the solution between steps
    Real nl_step_tol;
    /// Maximum number of iterations for the non-linear solver
    Int nl_max_iter;
    /// Relative convergence tolerance for the linear solver
    Real lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    Real lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    Int lin_max_iter;

public:
    static Parameters parameters();
};

} // namespace godzilla
