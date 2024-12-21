// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/Preconditioner.h"

namespace godzilla {

/// Linear problem
///
class LinearProblem : public Problem {
public:
    explicit LinearProblem(const Parameters & parameters);
    ~LinearProblem() override;
    void create() override;
    void run() override;

    /// true if solve converged, otherwise false
    bool converged();

protected:
    /// Get KSP type
    const std::string & get_ksp_type() const;

    virtual KrylovSolver create_krylov_solver();

    /// Initialize the problem
    virtual void init();

    /// Allocate Jacobian/residual objects
    void allocate_objects() override;

    /// Setup computation of residual and Jacobian callbacks
    virtual void set_up_callbacks();

    /// Setup monitors
    virtual void set_up_monitors();

    /// Setup solver parameters
    virtual void set_up_solver_parameters();

    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();

    /// Method for creating a preconditioner
    virtual Preconditioner create_preconditioner(PC pc);

    /// Monitor callback
    virtual void monitor(Int it, Real rnorm);

    /// Set KSP matrix evaluation function
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T to compute the operators
    template <class T>
    void
    set_compute_operators(T * instance, void (T::*method)(Matrix &, Matrix &))
    {
        this->compute_operators_delegate.bind(instance, method);
        PETSC_CHECK(DMKSPSetComputeOperators(get_dm(),
                                             invoke_compute_operators_delegate,
                                             &this->compute_operators_delegate));
    }

    template <class T>
    void
    set_compute_rhs(T * instance, void (T::*method)(Vector &))
    {
        this->compute_rhs_delegate.bind(instance, method);
        PETSC_CHECK(
            DMKSPSetComputeRHS(get_dm(), invoke_compute_rhs_delegate, &this->compute_rhs_delegate));
    }

private:
    /// Called before the solve
    virtual void pre_solve();

    /// Solve the linear problem
    virtual void solve();

    /// Called after the solve
    virtual void post_solve();

    /// KSP object
    KrylovSolver ks;
    /// Preconditioner
    Preconditioner pc;
    /// KSP type
    std::string ksp_type;
    /// Relative convergence tolerance for the linear solver
    Real lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    Real lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    Int lin_max_iter;
    /// Delegate for the compute_operators method
    Delegate<void(Matrix &, Matrix &)> compute_operators_delegate;
    /// Delegate for the compute_operators method
    Delegate<void(Vector &)> compute_rhs_delegate;

public:
    static Parameters parameters();

private:
    static ErrorCode invoke_compute_operators_delegate(KSP, Mat, Mat, void *);
    static ErrorCode invoke_compute_rhs_delegate(KSP, Vec, void *);
};

} // namespace godzilla
