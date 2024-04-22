// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Preconditioner.h"
#include "petscsnes.h"

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
    KSP get_ksp() const;
    /// Set KSP operators
    void set_ksp_operators(const Matrix & A, const Matrix & B);
    /// Get Jacobian matrix
    const Matrix & get_jacobian() const;
    /// true if solve converged, otherwise false
    virtual bool converged();
    /// Method to compute residual. Called from the PETsc callback
    virtual PetscErrorCode compute_residual(const Vector & x, Vector & f);
    /// Method to compute Jacobian. Called from the PETsc callback
    virtual PetscErrorCode compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp);

    /// Use matrix free finite difference matrix vector products to apply the Jacobian
    ///
    /// @param mf_operator use matrix-free only the A matrix
    /// @param mf use matrix-free for both the A and P matrices
    void set_use_matrix_free(bool mf_operator, bool mf);

protected:
    /// Get underlying SNES
    SNES get_snes() const;
    /// Set SNES object for this non-linear problem
    void set_snes(SNES snes);
    /// Set Jacobian evaluation function
    void set_jacobian_function(PetscErrorCode (*jacobian_func)(SNES, Vec, Mat, Mat, void *),
                               void * ctx);
    /// Set residual vector
    void set_residual_vector(const Vector & f);
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
    /// SNES monitor
    void snes_monitor(Int it, Real norm);
    /// KSP monitor
    void ksp_monitor(Int it, Real rnorm);
    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();
    /// Method for creating a preconditioner
    virtual Preconditioner create_preconditioner(PC pc);
    /// Solve the problem
    virtual void solve();

private:
    void set_up_preconditioning();

    /// SNES object
    SNES snes;
    /// KSP object
    KSP ksp;
    /// The residual vector
    Vector r;
    /// Jacobian matrix
    Matrix J;
    /// Converged reason
    SNESConvergedReason converged_reason;
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

    friend PetscErrorCode __ksp_monitor(KSP ksp, Int it, Real rnorm, void * ctx);
    friend PetscErrorCode __snes_monitor(SNES snes, Int it, Real norm, void * ctx);
};

} // namespace godzilla
