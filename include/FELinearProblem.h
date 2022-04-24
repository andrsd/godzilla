#pragma once

#include "Problem.h"
#include "Forms.h"
#include "petscksp.h"

namespace godzilla {

class Mesh;
class Shapeset;
class Space;
class PetscVector;
class PetscMatrix;
class ShapeFunction1D;
class RefMap1D;
class QPoint1D;

/// PETSc Linear problem
///
class FELinearProblem : public Problem {
public:
    FELinearProblem(const InputParameters & parameters);
    virtual ~FELinearProblem();

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
    virtual void setup_callbacks();
    /// Setup monitors
    virtual void setup_monitors();
    /// Setup solver parameters
    virtual void setup_solver_parameters();
    /// Method to compute right-hand side. Called from the PETsc callback
    virtual PetscErrorCode compute_rhs_callback(Vec b);
    /// Method to compute operators. Called from the PETsc callback
    virtual PetscErrorCode compute_operators_callback(Mat A, Mat B);
    /// KSP monitor
    PetscErrorCode ksp_monitor_callback(PetscInt it, PetscReal rnorm);
    ///
    virtual void assemble(PetscMatrix * matrix, PetscVector * rhs);

    /// Add a variable
    ///
    /// @param[in] name Name of the variable
    /// @param[in] nc Number of components
    /// @param[in] p Polynomial order
    virtual void add_variable(const std::string & name, uint nc, uint p);
    /// Set boundary condition information
    virtual void set_bc_information();
    /// Assign DoFs
    virtual void assign_dofs();
    /// Update constraints
    virtual void update_constraints();
    /// Set up field variables
    virtual void on_set_fields() = 0;
    /// setup wek form
    virtual void on_set_weak_form() = 0;
    /// Method for setting matrix properties
    virtual void on_set_matrix_properties();

    /// Unstructured mesh
    Mesh * mesh;
    /// Shapeset
    Shapeset * shapeset;
    /// Spaces
    std::vector<Space *> spaces;
    /// Map from varaible name to an index into the `spaces` array
    std::map<std::string, std::size_t> space_names;

    /// PETSc section
    // TODO: better name
    PetscSection section;
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

    SFn1D * get_fn(ShapeFunction1D * shfn, RefMap1D * rm, const uint np, const QPoint1D * pts);

    PetscScalar eval_bilin_form(uint np, Real *jxw, SFn1D *u, SFn1D * v);
    PetscScalar eval_lin_form(uint np, Real * jxw, SFn1D * v);

public:
    static InputParameters validParams();

    friend PetscErrorCode __compute_rhs(KSP ksp, Vec b, void * ctx);
    friend PetscErrorCode __compute_operators(KSP ksp, Mat A, Mat B, void * ctx);
    friend PetscErrorCode __ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx);
};

} // namespace godzilla
