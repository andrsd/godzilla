// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscksp.h"
#include "godzilla/Types.h"
#include "godzilla/Error.h"

namespace godzilla {

class Matrix;
class Vector;

namespace internal {

/// Abstract "method" for calling KSP monitor
struct KSPMonitorMethodAbstract {
    virtual ~KSPMonitorMethodAbstract() = default;
    virtual ErrorCode invoke(Int it, Real rnorm) = 0;
};

template <typename T>
struct KSPMonitorMethod : public KSPMonitorMethodAbstract {
    KSPMonitorMethod(T * instance, ErrorCode (T::*monitor)(Int, Real)) :
        instance(instance),
        monitor(monitor)
    {
    }

    ErrorCode
    invoke(Int it, Real rnorm) override
    {
        return ((*this->instance).*monitor)(it, rnorm);
    }

private:
    T * instance;
    ErrorCode (T::*monitor)(Int, Real);
};

/// Abstract "method" for calling KSP compute RHS
struct KSPComputeRhsMethodAbstract {
    virtual ~KSPComputeRhsMethodAbstract() = default;
    virtual ErrorCode invoke(Vector & b) = 0;
};

template <typename T>
struct KSPComputeRhsMethod : public KSPComputeRhsMethodAbstract {
    KSPComputeRhsMethod(T * instance, ErrorCode (T::*method)(Vector & b)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Vector & b) override
    {
        return ((*this->instance).*method)(b);
    }

private:
    T * instance;
    ErrorCode (T::*method)(Vector &);
};

/// Abstract "method" for calling KSP compute operators
struct KSPComputeOperatorsMethodAbstract {
    virtual ~KSPComputeOperatorsMethodAbstract() = default;
    virtual ErrorCode invoke(Matrix & A, Matrix & B) = 0;
};

template <typename T>
struct KSPComputeOperatorsMethod : public KSPComputeOperatorsMethodAbstract {
    KSPComputeOperatorsMethod(T * instance, ErrorCode (T::*method)(Matrix &, Matrix &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Matrix & A, Matrix & B) override
    {
        return ((*this->instance).*method)(A, B);
    }

private:
    T * instance;
    ErrorCode (T::*method)(Matrix &, Matrix &);
};

} // namespace internal

/// Wrapper around KSP
class KrylovSolver {
public:
    enum ConvergedReason {
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
        CONVERGED_HAPPY_BREAKDOWN = KSP_CONVERGED_HAPPY_BREAKDOWN
    };

    /// Construct empty Krylov solver
    KrylovSolver();
    ~KrylovSolver();

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

    /// Set routine to compute the right hand side of the linear system
    ///
    /// @param func Function to compute the right hand side
    /// @param ctx Optional context
    void set_compute_rhs(ErrorCode (*func)(KSP ksp, Vec b, void * ctx), void * ctx = nullptr);

    /// Set member function to compute the right hand side of the linear system
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param callback Member function in class T
    template <class T>
    void
    set_compute_rhs(T * instance, ErrorCode (T::*callback)(Vector &))
    {
        this->compute_rhs_method = new internal::KSPComputeRhsMethod<T>(instance, callback);
        PETSC_CHECK(KSPSetComputeRHS(this->ksp, compute_rhs, this->compute_rhs_method));
    }

    /// Set routine to compute the linear operators
    ///
    /// @param func Function to compute the operators
    /// @param ctx Optional context
    void set_compute_operators(ErrorCode (*func)(KSP ksp, Mat A, Mat B, void * ctx),
                               void * ctx = nullptr);

    /// Set member function to compute operators of the linear system
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param callback Member function in class T
    template <class T>
    void
    set_compute_operators(T * instance, ErrorCode (T::*callback)(Matrix &, Matrix &))
    {
        this->compute_operators_method =
            new internal::KSPComputeOperatorsMethod<T>(instance, callback);
        PETSC_CHECK(
            KSPSetComputeOperators(this->ksp, compute_operators, this->compute_operators_method));
    }

    /// Sets an *additional* function to be called at every iteration to monitor the residual/error
    /// etc.
    ///
    /// @param monitor Pointer to function (if this is `nullptr`, it turns off monitoring)
    /// @param ctx Context for private data for the monitor routine (use `nullptr` if no context is
    /// needed)
    /// @param monitordestroy Routine that frees monitor context (may be `nullptr`)
    void monitor_set(ErrorCode (*monitor)(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx),
                     void * ctx = nullptr,
                     ErrorCode (*monitordestroy)(void ** ctx) = nullptr);

    /// Sets an *additional* member function to be called at every iteration to monitor the
    /// residual/error etc.
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of class T
    /// @param callback Member function in class T
    template <class T>
    void
    monitor_set(T * instance, ErrorCode (T::*callback)(Int, Real))
    {
        this->monitor_method = new internal::KSPMonitorMethod<T>(instance, callback);
        PETSC_CHECK(KSPMonitorSet(this->ksp, monitor, this->monitor_method, monitor_destroy));
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

private:
    /// PETSc object
    KSP ksp;
    /// Method for monitoring the solve
    internal::KSPMonitorMethodAbstract * monitor_method;
    /// Method for computing RHS
    internal::KSPComputeRhsMethodAbstract * compute_rhs_method;
    /// Method for computing operators
    internal::KSPComputeOperatorsMethodAbstract * compute_operators_method;

public:
    static ErrorCode compute_operators(KSP, Mat A, Mat B, void * ctx);
    static ErrorCode compute_rhs(KSP, Vec b, void * ctx);
    static ErrorCode monitor(KSP, Int it, Real rnorm, void * ctx);
    static ErrorCode monitor_destroy(void ** ctx);
};

} // namespace godzilla
