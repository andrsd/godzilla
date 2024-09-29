// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/Delegate.h"
#include "petscsnes.h"

namespace godzilla {

/// Wrapper around SNES
class SNESolver {
public:
    enum ConvergedReason {
        CONVERGED_ITERATING = SNES_CONVERGED_ITERATING,
        //
        CONVERGED_FNORM_ABS = SNES_CONVERGED_FNORM_ABS,
        CONVERGED_FNORM_RELATIVE = SNES_CONVERGED_FNORM_RELATIVE,
        CONVERGED_SNORM_RELATIVE = SNES_CONVERGED_SNORM_RELATIVE,
        CONVERGED_ITS = SNES_CONVERGED_ITS,
        BREAKOUT_INNER_ITER = SNES_BREAKOUT_INNER_ITER,
        //
        DIVERGED_FUNCTION_DOMAIN = SNES_DIVERGED_FUNCTION_DOMAIN,
        DIVERGED_FUNCTION_COUNT = SNES_DIVERGED_FUNCTION_COUNT,
        DIVERGED_LINEAR_SOLVE = SNES_DIVERGED_LINEAR_SOLVE,
        DIVERGED_FNORM_NAN = SNES_DIVERGED_FNORM_NAN,
        DIVERGED_MAX_IT = SNES_DIVERGED_MAX_IT,
        DIVERGED_LINE_SEARCH = SNES_DIVERGED_LINE_SEARCH,
        DIVERGED_INNER = SNES_DIVERGED_INNER,
        DIVERGED_LOCAL_MIN = SNES_DIVERGED_LOCAL_MIN,
        DIVERGED_DTOL = SNES_DIVERGED_DTOL,
        DIVERGED_JACOBIAN_DOMAIN = SNES_DIVERGED_JACOBIAN_DOMAIN,
        DIVERGED_TR_DELTA = SNES_DIVERGED_TR_DELTA,
    };

    /// Wrapper around SNESLineSearch
    class LineSearch {
    public:
        enum LineSearchType { BASIC, L2, CP, NLEQERR, SHELL, BT };

        LineSearch(SNESLineSearch ls);

        void set_type(LineSearchType type);

        void set_from_options();

        operator SNESLineSearch() const;

    private:
        SNESLineSearch ls;
    };

    /// Construct empty non-linear solver
    SNESolver();
    ~SNESolver() = default;

    /// Construct a non-linear solver from a PETSc SNES object
    explicit SNESolver(SNES snes);

    /// Create a solver
    void create(MPI_Comm comm);

    /// Destroy the solver
    void destroy();

    [[nodiscard]] KrylovSolver get_ksp() const;

    [[nodiscard]] LineSearch get_line_search() const;

    void set_line_search(LineSearch ls);

    /// Sets the DM that may be used by some nonlinear solvers or their underlying preconditioners
    ///
    /// @param dm The `DM`
    void set_dm(DM dm);

    /// Sets the method for the nonlinear solver.
    ///
    /// @param A known method
    void set_type(SNESType type);

    /// Gets the SNES method type (as a string).
    ///
    /// @return SNES method
    std::string get_type() const;

    /// Sets non-linear solver options from the options database
    void set_from_options();

    /// Sets the function evaluation routine and function vector for use by the SNES routines in
    /// solving systems of nonlinear equations.
    ///
    /// @tparam T C++ class type
    /// @param r Vector to store function values
    /// @param instance Instance of class T
    /// @param method Member function in class T to compute function values
    template <class T>
    void
    set_function(Vector & r, T * instance, void (T::*method)(const Vector &, Vector &))
    {
        this->compute_residual_method.bind(instance, method);
        PETSC_CHECK(SNESSetFunction(this->snes,
                                    r,
                                    invoke_compute_residual_delegate,
                                    &this->compute_residual_method));
    }

    /// Sets the function to compute Jacobian as well as the location to store the matrix.
    ///
    /// @tparam T C++ class type
    /// @param J The matrix that defines the (approximate) Jacobian
    /// @param Jp The matrix to be used in constructing the preconditioner, usually the same as `J`.
    /// @param instance Instance of class T
    /// @param method Member function in class T to compute the Jacobian
    template <class T>
    void
    set_jacobian(Matrix & J,
                 Matrix & Jp,
                 T * instance,
                 void (T::*method)(const Vector &, Matrix &, Matrix &))
    {
        this->compute_jacobian_method.bind(instance, method);
        PETSC_CHECK(SNESSetJacobian(this->snes,
                                    J,
                                    Jp,
                                    invoke_compute_jacobian_delegate,
                                    &this->compute_jacobian_method));
    }

    /// Indicates that the solver should use matrix-free finite difference matrix-vector products to
    /// apply the Jacobian.
    ///
    /// @param mf_operator Use matrix-free only for the `J` used by `set_jacobian`, this means the
    /// user provided `Jp` will continue to be used
    /// @param mf Use matrix-free for both the `J` and `Jp` used by `set_jacobian`, both the J
    /// and Jp set in `set_jacobian` will be ignored. With this option no matrix-element based
    /// preconditioners can be used in the linear solve since the matrix won’t be explicitly
    /// available
    void set_use_matrix_free(bool mf_operator, bool mf);

    /// Sets various parameters used in convergence tests.
    ///
    /// @param abs_tol Absolute convergence tolerance
    /// @param rtol Relative convergence tolerance
    /// @param stol Convergence tolerance in terms of the norm of the change in the solution between
    ///  steps, $|| delta x || < stol*|| x ||$
    /// @param max_it Maximum number of iterations, default 50.
    /// @param maxf Maximum number of function evaluations (-1 indicates no limit), default 1000
    void set_tolerances(Real abs_tol, Real rtol, Real stol, Int max_it, Int maxf);

    /// Sets an *additional* member function that is to be used at every iteration of the nonlinear
    /// solver residual/error etc.
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    monitor_set(T * instance, void (T::*method)(Int, Real))
    {
        this->monitor_method.bind(instance, method);
        PETSC_CHECK(
            SNESMonitorSet(this->snes, invoke_monitor_delegate, &this->monitor_method, nullptr));
    }

    /// Solves a nonlinear system F(x) = b.
    ///
    /// @param b The constant part of the equation F(x) = b
    /// @param x The solution vector
    void solve(const Vector & b, Vector & x) const;

    /// Solves a nonlinear system F(x) = 0.
    ///
    /// @param x The solution vector
    void solve(Vector & x) const;

    /// Gets the reason the KSP iteration was stopped
    ///
    /// @return
    [[nodiscard]] ConvergedReason get_converged_reason() const;

    /// Creates a finite differencing based matrix-free matrix context for use with a SNES solver.
    ///
    /// @return The matrix-free matrix which is of type `MATMFFD`
    Matrix mat_create_mf() const;

    /// typecast operator so we can use our class directly with PETSc API
    operator SNES() const;

private:
    /// PETSc object
    SNES snes;
    /// Method for monitoring the solve
    Delegate<void(Int it, Real rnorm)> monitor_method;
    /// Method for computing residual
    Delegate<void(const Vector & x, Vector & f)> compute_residual_method;
    /// Method for computing Jacobian
    Delegate<void(const Vector & x, Matrix & J, Matrix & Jp)> compute_jacobian_method;

public:
    static ErrorCode invoke_compute_residual_delegate(SNES, Vec, Vec, void *);
    static ErrorCode invoke_compute_jacobian_delegate(SNES, Vec, Mat, Mat, void *);
    static ErrorCode invoke_monitor_delegate(SNES, Int, Real, void *);
};

} // namespace godzilla
