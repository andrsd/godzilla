#pragma once

#include "Problem.h"
#include "Vector.h"
#include "Matrix.h"
#include "petscsnes.h"

namespace godzilla {

/// PETSc non-linear problem
///
class NonlinearProblem : public Problem {
public:
    explicit NonlinearProblem(const Parameters & parameters);
    ~NonlinearProblem() override;

    void create() override;
    void check() override;
    void run() override;
    void solve() override;
    bool converged() override;
    const Vector & get_solution_vector() const override;
    /// Method to compute residual. Called from the PETsc callback
    virtual PetscErrorCode compute_residual(const Vector & x, Vector & f);
    /// Method to compute Jacobian. Called from the PETsc callback
    virtual PetscErrorCode compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp);

protected:
    /// provide DM for the underlying SNES object
    DM get_dm() const override;
    /// Initialize the problem
    virtual void init();
    /// Set up initial guess
    virtual void set_up_initial_guess();
    /// Allocate Jacobian/residual objects
    virtual void allocate_objects();
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
    PetscErrorCode snes_monitor_callback(Int it, Real norm);
    /// KSP monitor
    PetscErrorCode ksp_monitor_callback(Int it, Real rnorm);
    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();
    /// Method for setting preconditioning
    virtual void set_up_preconditioning();

    /// SNES object
    SNES snes;
    /// KSP object
    KSP ksp;
    /// The solution vector
    Vector x;
    /// The residual vector
    Vector r;
    /// Jacobian matrix
    Matrix J;
    /// Converged reason
    SNESConvergedReason converged_reason;

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
