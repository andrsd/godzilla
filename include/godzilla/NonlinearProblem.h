// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/SNESolver.h"

namespace godzilla {

/// Nonlinear problem
///
class NonlinearProblem : public Problem {
public:
    explicit NonlinearProblem(const Parameters & parameters);
    ~NonlinearProblem() override;

    void create() override;
    void run() override;

    /// Get underlying KSP
    [[nodiscard]] KrylovSolver get_ksp() const;

    /// Set KSP operators
    void set_ksp_operators(const Matrix & A, const Matrix & B);

    /// Get Jacobian matrix
    [[nodiscard]] const Matrix & get_jacobian() const;

    /// true if solve converged, otherwise false
    bool converged();

    /// Use matrix free finite difference matrix vector products to apply the Jacobian
    ///
    /// @param mf_operator use matrix-free only the A matrix
    /// @param mf use matrix-free for both the A and P matrices
    void set_use_matrix_free(bool mf_operator, bool mf);

protected:
    /// Get underlying non-linear solver
    [[nodiscard]] SNESolver get_snes() const;

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

    /// Set up computation of residual and Jacobian callbacks
    virtual void set_up_callbacks();

    /// Set up monitors
    virtual void set_up_monitors();

    /// Set up solve type
    virtual void set_up_solve_type();

    /// SNES monitor
    virtual void snes_monitor(Int it, Real norm);

    /// KSP monitor
    virtual void ksp_monitor(Int it, Real rnorm);

private:
    /// Set up line search
    virtual void set_up_line_search();

    /// Set up solver parameters
    virtual void set_up_solver_parameters();

    /// Method for creating a preconditioner
    virtual Preconditioner create_preconditioner(PC pc);

    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();

    virtual void compute_residual(const Vector & x, Vector & f) = 0;
    virtual void compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp) = 0;

    /// Nonlinear solver
    SNESolver snes;
    /// Linear solver
    KrylovSolver ksp;
    /// Residual vector
    Vector r;
    /// Jacobian matrix
    Matrix J;
    /// Converged reason
    SNESolver::ConvergedReason converged_reason;
    /// Preconditioner
    Preconditioner pc;
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
