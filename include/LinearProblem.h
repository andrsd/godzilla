#pragma once

#include "Problem.h"
#include "petscksp.h"

namespace godzilla {

/// PETSc Linear problem
///
class LinearProblem : public Problem {
public:
    LinearProblem(const InputParameters & parameters);
    virtual ~LinearProblem();

    virtual void create() override;
    virtual void solve() override;
    virtual void run() override;
    virtual bool converged() override;
    virtual Vec get_solution_vector() const override;

protected:
    /// provide DM for the underlying KSP object
    virtual DM get_dm() const override;
    /// Initialize the problem
    virtual void init();
    /// Allocate Jacobian/residual objects
    virtual void allocate_objects();
    /// Setup computation of residual and Jacobian callbacks
    virtual void set_up_callbacks();
    /// Setup monitors
    virtual void set_up_monitors();
    /// Setup solver parameters
    virtual void set_up_solver_parameters();
    /// Method to compute right-hand side. Called from the PETsc callback
    virtual PetscErrorCode compute_rhs_callback(Vec b) = 0;
    /// Method to compute operators. Called from the PETsc callback
    virtual PetscErrorCode compute_operators_callback(Mat A, Mat B) = 0;
    /// KSP monitor
    PetscErrorCode ksp_monitor_callback(PetscInt it, PetscReal rnorm);

    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();

    /// KSP object
    KSP ksp;
    /// The solution vector
    Vec x;
    /// The right-hand side vector
    Vec b;
    /// Linear operator matrix
    Mat A;
    /// Preconditioning matrix
    Mat B;
    /// Converged reason
    KSPConvergedReason converged_reason;

    /// Relative convergence tolerance for the linear solver
    PetscReal lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    PetscReal lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    PetscInt lin_max_iter;

public:
    static InputParameters valid_params();

    friend PetscErrorCode __compute_rhs(KSP ksp, Vec b, void * ctx);
    friend PetscErrorCode __compute_operators(KSP ksp, Mat A, Mat B, void * ctx);
    friend PetscErrorCode __ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx);
};

} // namespace godzilla
