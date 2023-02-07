#pragma once

#include "GodzillaConfig.h"
#include "Problem.h"
#include "Vector.h"
#include "Matrix.h"
#include "petscksp.h"

namespace godzilla {

/// PETSc Linear problem
///
class LinearProblem : public Problem {
public:
    explicit LinearProblem(const Parameters & parameters);
    ~LinearProblem() override;

    void create() override;
    void solve() override;
    void run() override;
    bool converged() override;
    NO_DISCARD const Vector & get_solution_vector() const override;

protected:
    /// provide DM for the underlying KSP object
    NO_DISCARD DM get_dm() const override;
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
    virtual PetscErrorCode compute_rhs_callback(Vector & b) = 0;
    /// Method to compute operators. Called from the PETsc callback
    virtual PetscErrorCode compute_operators_callback(Matrix & A, Matrix & B) = 0;
    /// KSP monitor
    PetscErrorCode ksp_monitor_callback(Int it, Real rnorm);
    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();
    /// Method for setting preconditioning
    virtual void set_up_preconditioning();

    /// KSP object
    KSP ksp;
    /// The solution vector
    Vector x;
    /// The right-hand side vector
    Vector b;
    /// Linear operator matrix
    Matrix A;
    /// Converged reason
    KSPConvergedReason converged_reason;

    /// Relative convergence tolerance for the linear solver
    Real lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    Real lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    Int lin_max_iter;

public:
    static Parameters parameters();

    friend PetscErrorCode __compute_rhs(KSP ksp, Vec b, void * ctx);
    friend PetscErrorCode __compute_operators(KSP ksp, Mat A, Mat B, void * ctx);
    friend PetscErrorCode __ksp_monitor_linear(KSP ksp, Int it, Real rnorm, void * ctx);
};

} // namespace godzilla
