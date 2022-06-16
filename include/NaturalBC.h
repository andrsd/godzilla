#pragma once

#include "BoundaryCondition.h"
#include "Types.h"

namespace godzilla {

/// Base class for natural boundary conditions
class NaturalBC : public BoundaryCondition {
public:
    NaturalBC(const InputParameters & params);

    virtual DMBoundaryConditionType get_bc_type() const override;

protected:
    /// Set residual statement for the boundary integral
    ///
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void set_residual_block(PetscFEBndResidualFunc * f0, PetscFEBndResidualFunc * f1);

    /// Set Jacobian statement for the boundary integral
    ///
    /// @param gid Field ID
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void set_jacobian_block(PetscInt gid,
                            PetscFEBndJacobianFunc * g0,
                            PetscFEBndJacobianFunc * g1,
                            PetscFEBndJacobianFunc * g2,
                            PetscFEBndJacobianFunc * g3);

    virtual void set_up_callback() override;

    /// Set up the weak form for the boundary integral of this boundary condition
    virtual void set_up_weak_form() = 0;

    /// WeakForm object
    PetscWeakForm wf;

    /// Boundary number
    PetscInt bd;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
