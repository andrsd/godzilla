// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Problem.h"
#include "godzilla/RestartInterface.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/SNESolver.h"

namespace godzilla {

/// Nonlinear problem
///
class NonlinearProblem : public Problem, public RestartInterface {
public:
    explicit NonlinearProblem(const Parameters & parameters);
    ~NonlinearProblem() override;

    void create() override;
    void run() override;
    void write_restart_file(RestartFile & file) const override;
    void read_restart_file(const RestartFile & file) override;

    /// Get underlying non-linear solver
    const SNESolver & get_snes() const;

    SNESolver & get_snes();

    /// Get underlying KSP
    const KrylovSolver & get_ksp() const;

    KrylovSolver & get_ksp();

    /// Set KSP operators
    void set_ksp_operators(const Matrix & A, const Matrix & B);

    /// Get Jacobian matrix
    const Matrix & get_jacobian() const;

    Matrix & get_jacobian();

    /// true if solve converged, otherwise false
    bool converged();

    /// Use matrix free finite difference matrix vector products to apply the Jacobian
    ///
    /// @param mf_operator use matrix-free only the A matrix
    /// @param mf use matrix-free for both the A and P matrices
    void set_use_matrix_free(bool mf_operator, bool mf);

    /// Called before the solve
    virtual void pre_solve();

    /// Solve the problem
    void solve();

    /// Called after the solve
    virtual void post_solve();

protected:
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
    void snes_monitor(Int it, Real norm);

    /// KSP monitor
    void ksp_monitor(Int it, Real rnorm);

    /// Set residual evaluation function
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T to compute residual
    template <class T>
    void
    set_function(T * instance, void (T::*method)(const Vector &, Vector &))
    {
        this->snes.set_function(this->r, instance, method);
    }

    /// Set Jacobian evaluation function
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T to compute Jacobian
    template <class T>
    void
    set_jacobian(T * instance, void (T::*method)(const Vector &, Matrix &, Matrix &))
    {
        this->snes.set_jacobian(this->J, this->J, instance, method);
    }

private:
    /// Create a SNESolver
    virtual SNESolver create_sne_solver();

    /// Set up line search
    virtual void set_up_line_search();

    /// Set up solver parameters
    virtual void set_up_solver_parameters();

    /// Method for creating a preconditioner
    virtual Preconditioner create_preconditioner(PC pc);

    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();

    /// Nonlinear solver
    SNESolver snes;
    /// Linear solver
    KrylovSolver ksp;
    /// Residual vector
    Vector r;
    /// Jacobian matrix
    Matrix J;
    /// Preconditioner
    Preconditioner pcond;
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
