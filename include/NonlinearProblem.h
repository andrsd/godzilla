#pragma once

#include "Problem.h"
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
    Vec get_solution_vector() const override;
    /// Method to compute residual. Called from the PETsc callback
    virtual PetscErrorCode compute_residual(Vec x, Vec f);
    /// Method to compute Jacobian. Called from the PETsc callback
    virtual PetscErrorCode compute_jacobian(Vec x, Mat J, Mat Jp);

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
    /// Set up solver parameters
    virtual void set_up_solver_parameters();
    /// SNES monitor
    PetscErrorCode snes_monitor_callback(Int it, PetscReal norm);
    /// KSP monitor
    PetscErrorCode ksp_monitor_callback(Int it, PetscReal rnorm);
    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();
    /// Method for setting preconditioning
    virtual void set_up_preconditioning();

    /// SNES object
    SNES snes;
    /// KSP object
    KSP ksp;
    /// The solution vector
    Vec x;
    /// The residual vector
    Vec r;
    /// Jacobian matrix
    Mat J;
    /// Converged reason
    SNESConvergedReason converged_reason;

    /// The type of line search to be used
    std::string line_search_type;
    /// Relative convergence tolerance for the non-linear solver
    PetscReal nl_rel_tol;
    /// Absolute convergence tolerance for the non-linear solver
    PetscReal nl_abs_tol;
    /// Convergence tolerance in terms of the norm of the change in the solution between steps
    PetscReal nl_step_tol;
    /// Maximum number of iterations for the non-linear solver
    Int nl_max_iter;
    /// Relative convergence tolerance for the linear solver
    PetscReal lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    PetscReal lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    Int lin_max_iter;

public:
    static Parameters parameters();

    friend PetscErrorCode __ksp_monitor(KSP ksp, Int it, PetscReal rnorm, void * ctx);
    friend PetscErrorCode __snes_monitor(SNES snes, Int it, PetscReal norm, void * ctx);
};

} // namespace godzilla
