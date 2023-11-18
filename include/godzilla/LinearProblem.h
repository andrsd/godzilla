#pragma once

#include "godzilla/GodzillaConfig.h"
#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "petscksp.h"

namespace godzilla {

/// PETSc Linear problem
///
class LinearProblem : public Problem {
public:
    explicit LinearProblem(const Parameters & parameters);
    ~LinearProblem() override;

    void create() override;
    void run() override;
    NO_DISCARD const Vector & get_solution_vector() const override;

    /// true if solve converged, otherwise false
    virtual bool converged();
    /// Method to compute right-hand side. Called from the PETsc callback
    virtual PetscErrorCode compute_rhs(Vector & b);
    /// Method to compute operators. Called from the PETsc callback
    virtual PetscErrorCode compute_operators(Matrix & A, Matrix & B);

protected:
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
    /// KSP monitor
    void ksp_monitor_callback(Int it, Real rnorm);
    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();
    /// Method for setting preconditioning
    virtual void set_up_preconditioning();
    /// Solve the problem
    virtual void solve();

private:
    /// KSP object
    KSP ksp;
    /// The solution vector
    Vector x;
    /// The right-hand side vector
    Vector b;
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

    friend PetscErrorCode __ksp_monitor_linear(KSP ksp, Int it, Real rnorm, void * ctx);
};

} // namespace godzilla
