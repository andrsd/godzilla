#pragma once

#include "Problem.h"
#include "Forms.h"
#include "AssemblyList.h"
#include "petscksp.h"

namespace godzilla {

class Mesh;
class Shapeset;
class Space;
class BoundaryCondition;
class PetscVector;
class PetscMatrix;
class ShapeFunction1D;
class RefMap1D;
class QPoint1D;
class Fn1D;
class Gradient1D;

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

    const uint
    get_num_points() const
    {
        return this->np;
    }
    const Real *
    get_jxw() const
    {
        return this->jxw;
    }
    const Real *
    get_u(uint idx) const
    {
        return this->u[idx].get_values();
    }
    const Real *
    get_v(uint idx) const
    {
        return this->v[idx].get_values();
    }
    const RealVector1D *
    get_grad_u(uint idx) const
    {
        return this->grad_u[idx].get_values();
    }
    const RealVector1D *
    get_grad_v(uint idx) const
    {
        return this->grad_v[idx].get_values();
    }

    const AssemblyList *
    get_assembly_list(uint idx) const
    {
        return this->al + idx;
    }

    virtual void add_boundary_condition(BoundaryCondition * bc);

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
    /// List of boundary condition objects
    std::vector<BoundaryCondition *> bcs;
    /// Number of degrees of freedom in the system
    uint n_dofs;
    ///
    uint dof_stride;
    /// Reference map
    RefMap1D * refmap;
    /// Number of equations
    uint neq;
    /// Assembly lists
    AssemblyList * al;
    /// Base functions (one per equation/space)
    ShapeFunction1D ** base_fn;
    /// Test functions (one per equation/space)
    ShapeFunction1D ** test_fn;
    //
    uint np;
    Real * jxw;
    Fn1D * u;
    Gradient1D * grad_u;
    Fn1D * v;
    Gradient1D * grad_v;

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
    Real lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    Real lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    PetscInt lin_max_iter;

public:
    static InputParameters validParams();

    friend PetscErrorCode __compute_rhs(KSP ksp, Vec b, void * ctx);
    friend PetscErrorCode __compute_operators(KSP ksp, Mat A, Mat B, void * ctx);
    friend PetscErrorCode __ksp_monitor_linear(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx);
};

} // namespace godzilla
