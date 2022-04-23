#pragma once

#include "Problem.h"
#include "petscsnes.h"

namespace godzilla {

class Mesh;

/// Nonlinear finite element problem
class FENonlinearProblem : public Problem {
public:
    FENonlinearProblem(const InputParameters & parameters);
    virtual ~FENonlinearProblem();

    virtual DM get_dm() const override;
    virtual void create() override;
    virtual void run() override;
    virtual void solve() override;
    virtual bool converged() override;
    Vec get_solution_vector() const override;

protected:
    /// Initialize the problem
    virtual void init();
    /// Set up initial guess
    virtual void setup_initial_guess();
    /// Allocate Jacobian/residual objects
    virtual void allocate_objects();
    /// Set up line search
    virtual void setup_line_search();
    /// Set up computation of residual and Jacobian callbacks
    virtual void setup_callbacks();
    /// Set up monitors
    virtual void setup_monitors();
    /// Set up solver parameters
    virtual void setup_solver_parameters();
    /// Method to compute residual. Called from the PETsc callback
    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f);
    /// Method to compute Jacobian. Called from the PETsc callback
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp);
    /// SNES monitor
    PetscErrorCode snes_monitor_callback(PetscInt it, PetscReal norm);
    /// KSP monitor
    PetscErrorCode ksp_monitor_callback(PetscInt it, PetscReal rnorm);

    /// Set up field variables
    virtual void on_set_fields() = 0;
    /// setup wek form
    virtual void on_set_weak_form() = 0;
    /// Method for setting matrix properties
    virtual void on_set_matrix_properties();

    /// Unstructured mesh
    Mesh & mesh;
    /// SNES object
    SNES snes;
    /// The solution vector
    Vec x;
    /// The residual vector
    Vec r;
    /// Jacobian matrix
    Mat J;
    /// Preconditioning matrix
    Mat Jp;
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
    PetscInt nl_max_iter;
    /// Relative convergence tolerance for the linear solver
    PetscReal lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    PetscReal lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    PetscInt lin_max_iter;

public:
    static InputParameters validParams();

    friend PetscErrorCode __compute_residual(SNES snes, Vec x, Vec f, void * ctx);
    friend PetscErrorCode __compute_jacobian(SNES snes, Vec x, Mat A, Mat B, void * ctx);
    friend PetscErrorCode __ksp_monitor(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx);
    friend PetscErrorCode __snes_monitor(SNES snes, PetscInt it, PetscReal norm, void * ctx);
};

}
