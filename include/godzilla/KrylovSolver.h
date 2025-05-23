// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include "godzilla/Delegate.h"
#include "petscksp.h"
#include <tuple>

namespace godzilla {

class Matrix;
class Vector;

/// Wrapper around KSP
class KrylovSolver {
public:
    enum ConvergedReason {
        CONVERGED_ITERATING = KSP_CONVERGED_ITERATING,
        CONVERGED_RTOL_NORMAL = KSP_CONVERGED_RTOL_NORMAL,
        CONVERGED_ATOL_NORMAL = KSP_CONVERGED_ATOL_NORMAL,
        CONVERGED_RTOL = KSP_CONVERGED_RTOL,
        CONVERGED_ATOL = KSP_CONVERGED_ATOL,
        CONVERGED_ITS = KSP_CONVERGED_ITS,
#if PETSC_VERSION_GE(3, 19, 0)
        CONVERGED_NEG_CURVE = KSP_CONVERGED_NEG_CURVE,
#else
        CONVERGED_CG_NEG_CURVE = KSP_CONVERGED_CG_NEG_CURVE,
        CONVERGED_CG_CONSTRAINED = KSP_CONVERGED_CG_CONSTRAINED,
#endif
        CONVERGED_STEP_LENGTH = KSP_CONVERGED_STEP_LENGTH,
        CONVERGED_HAPPY_BREAKDOWN = KSP_CONVERGED_HAPPY_BREAKDOWN,
        // diverged reasons
        DIVERGED_NULL = KSP_DIVERGED_NULL,
        DIVERGED_ITS = KSP_DIVERGED_ITS,
        DIVERGED_DTOL = KSP_DIVERGED_DTOL,
        DIVERGED_BREAKDOWN = KSP_DIVERGED_BREAKDOWN,
        DIVERGED_BREAKDOWN_BICG = KSP_DIVERGED_BREAKDOWN_BICG,
        DIVERGED_NONSYMMETRIC = KSP_DIVERGED_NONSYMMETRIC,
        DIVERGED_INDEFINITE_PC = KSP_DIVERGED_INDEFINITE_PC,
        DIVERGED_NANORINF = KSP_DIVERGED_NANORINF,
        DIVERGED_INDEFINITE_MAT = KSP_DIVERGED_INDEFINITE_MAT,
        DIVERGED_PC_FAILED = KSP_DIVERGED_PC_FAILED
    };

    /// Construct empty Krylov solver
    KrylovSolver();

    /// Construct a Krylov solver from a PETSc KSP object
    explicit KrylovSolver(KSP ksp);

    /// Create a solver
    void create(MPI_Comm comm);

    /// Destroy the solver
    void destroy();

    /// Sets the DM that may be used by some preconditioners and that may be used to construct the
    /// linear system
    ///
    /// @param dm The `DM`
    void set_dm(DM dm);

    /// Sets the matrix associated with the linear system and a different one from which the
    /// preconditioner will be built
    ///
    /// @param A The matrix that defines the linear system
    /// @param B The matrix to be used in constructing the preconditioner
    void set_operators(const Matrix & A, const Matrix & B) const;

    /// Sets the matrix associated with the linear system
    ///
    /// @param A The matrix that defines the linear system
    void set_operator(const Matrix & A) const;

    /// Gets the matrix associated with the linear system
    ///
    /// @return The matrix associated with the linear system
    Matrix get_operator() const;

    /// Gets the matrix associated with the linear system and a (possibly) different one used to
    /// construct the preconditioner
    ///
    /// @return A tuple containing the matrix associated with the linear system and the matrix used
    ///           to construct the preconditioner
    std::tuple<Matrix, Matrix> get_operators() const;

    /// Sets KSP options from the options database
    void set_from_options();

    /// Sets the relative, absolute, divergence, and maximum iteration tolerances used by the
    /// default convergence testers
    ///
    /// @param rel_tol The relative convergence tolerance, relative decrease in the (possibly
    /// preconditioned) residual norm
    /// @param abs_tol The absolute convergence tolerance absolute size of the (possibly
    /// preconditioned) residual norm
    /// @param div_tol The divergence tolerance, amount (possibly preconditioned) residual norm can
    /// increase before `KSPConvergedDefault` concludes that the method is diverging
    /// @param max_its Maximum number of iterations to use
    void set_tolerances(Real rel_tol, Real abs_tol, Real div_tol, Int max_its);

    /// Gets the relative, absolute, divergence, and maximum iteration tolerances used by the
    /// default KSP convergence tests.
    ///
    /// @param rel_tol The relative convergence tolerance
    /// @param abs_tol The absolute convergence tolerance
    /// @param div_tol The divergence tolerance
    /// @param max_its The maximum number of iterations
    void get_tolerances(Real * rel_tol, Real * abs_tol, Real * div_tol, Int * max_its) const;

    /// Gets the right-hand-side vector for the linear system to be solved
    ///
    /// @return The right-hand-side vector
    Vector get_rhs() const;

    /// Gets the relative, absolute, divergence, and maximum iteration tolerances used by the
    /// default KSP convergence tests.
    ///
    /// @return A tuple containing the relative, absolute, divergence, and maximum number of
    /// iterations
    std::tuple<Real, Real, Real, Int> get_tolerances() const;

    /// Set member function to compute the right hand side of the linear system
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    set_compute_rhs(T * instance, void (T::*method)(Vector &))
    {
        this->compute_rhs_method.bind(instance, method);
        PETSC_CHECK(
            KSPSetComputeRHS(this->ksp, invoke_compute_rhs_delegate, &this->compute_rhs_method));
    }

    /// Set member function to compute operators of the linear system
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param method Member function in class T
    template <class T>
    void
    set_compute_operators(T * instance, void (T::*method)(Matrix &, Matrix &))
    {
        this->compute_operators_method.bind(instance, method);
        PETSC_CHECK(KSPSetComputeOperators(this->ksp,
                                           invoke_compute_operators_delegate,
                                           &this->compute_operators_method));
    }

    /// Sets an *additional* member function to be called at every iteration to monitor the
    /// residual/error etc.
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
            KSPMonitorSet(this->ksp, invoke_monitor_delegate, &this->monitor_method, nullptr));
    }

    template <class T>
    void
    set_convergence_test(T * instance, ConvergedReason (T::*method)(Int, Real))
    {
        this->convergence_test_method.bind(instance, method);
        PETSC_CHECK(KSPSetConvergenceTest(this->ksp,
                                          invoke_convergence_test_delegate,
                                          &this->convergence_test_method,
                                          nullptr));
    }

    /// Solve a linear system
    ///
    /// @param b The right-hand-side vector
    /// @param x The solution
    void solve(const Vector & b, Vector & x) const;

    /// Solve a linear system
    ///
    /// @param x The right-hand-side, after the solve it will contain the solution
    void solve(Vector & x) const;

    /// Gets the reason the KSP iteration was stopped
    ///
    /// @return
    ConvergedReason get_converged_reason() const;

    PC get_pc() const;

    /// typecast operator so we can use our class directly with PETSc API
    operator KSP() const;

    /// Set preconditioner type
    ///
    /// @tparam PCTYPE C++ class of a Preconditioner type
    /// @return Preconditioner class
    template <class PCTYPE>
    PCTYPE
    set_pc_type() const
    {
        return PCTYPE(get_pc());
    }

    /// Sets the preconditioning side
    ///
    /// @param side The preconditioning side, where side is one of `PC_LEFT`, `PC_RIGHT`,
    ///        `PC_SYMMETRIC`
    void set_pc_side(PCSide side);

    /// Tells the iterative solver that the initial guess is nonzero; otherwise KSP assumes the
    /// initial guess is to be zero (and thus zeros it out before solving).
    ///
    /// @param flag `true` indicates the guess is non-zero, `false` indicates the guess is zero
    void set_initial_guess_nonzero(bool flag);

    /// Sets up the internal data structures for the later use of an iterative solver.
    void set_up();

    /// Builds the KSP data structure for a particular KSPType
    ///
    /// @param type A krylov method
    void set_type(const char * type);

    /// Builds the KSP data structure for a particular KSPType
    ///
    /// @param type A krylov method
    void set_type(const std::string & type);

    /// View the KSP object
    ///
    /// @param viewer PETSc viewer
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

private:
    /// PETSc object
    KSP ksp;
    /// Method for monitoring the solve
    Delegate<void(Int it, Real rnorm)> monitor_method;
    /// Method for computing RHS
    Delegate<void(Vector & b)> compute_rhs_method;
    /// Method for computing operators
    Delegate<void(Matrix & A, Matrix & B)> compute_operators_method;
    /// Method for determining covergence
    Delegate<ConvergedReason(Int it, Real rnorm)> convergence_test_method;

public:
    static ErrorCode invoke_compute_operators_delegate(KSP, Mat A, Mat B, void * ctx);
    static ErrorCode invoke_compute_rhs_delegate(KSP, Vec b, void * ctx);
    static ErrorCode invoke_monitor_delegate(KSP, Int it, Real rnorm, void * ctx);
    static ErrorCode invoke_convergence_test_delegate(KSP,
                                                      Int it,
                                                      Real rnorm,
                                                      KSPConvergedReason * reason,
                                                      void * ctx);
};

} // namespace godzilla
