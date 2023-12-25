// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscksp.h"
#include "godzilla/Types.h"

namespace godzilla {

class Matrix;
class Vector;

/// Wrapper around KSP
class KrylovSolver {
public:
    // typedef void (*MonitorCallback)(Int, Real);

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
    void set_compute_rhs(PetscErrorCode (*func)(KSP ksp, Vec b, void * ctx), void * ctx = nullptr);

    /// Set routine to compute the linear operators
    ///
    /// @param func Function to compute the operators
    /// @param ctx Optional context
    void set_compute_operators(PetscErrorCode (*func)(KSP ksp, Mat A, Mat B, void * ctx),
                               void * ctx = nullptr);

    /// Sets an *additional* function to be called at every iteration to monitor the residual/error
    /// etc.
    ///
    /// @param monitor Pointer to function (if this is `nullptr`, it turns off monitoring)
    /// @param ctx Context for private data for the monitor routine (use `nullptr` if no context is
    /// needed)
    /// @param monitordestroy Routine that frees monitor context (may be `nullptr`)
    void monitor_set(PetscErrorCode (*monitor)(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx),
                     void * ctx = nullptr,
                     PetscErrorCode (*monitordestroy)(void ** ctx) = nullptr);

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

private:
    /// PETSc object
    KSP ksp;
    // /// Pointer to the monitor callback
    // MonitorCallback * monitor;
};

} // namespace godzilla
