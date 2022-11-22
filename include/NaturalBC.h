#pragma once

#include "BoundaryCondition.h"
#include "Types.h"

namespace godzilla {

class WeakForm;
class BndResidualFunc;
class BndJacobianFunc;

/// Base class for natural boundary conditions
class NaturalBC : public BoundaryCondition {
public:
    NaturalBC(const Parameters & params);

    /// Set up the weak form for the boundary integral of this boundary condition
    virtual void set_up_weak_form() = 0;

protected:
    /// Set residual statement for the boundary integral
    ///
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void set_residual_block(BndResidualFunc * f0, BndResidualFunc * f1);

    /// Set Jacobian statement for the boundary integral
    ///
    /// @param gid Field ID
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void set_jacobian_block(PetscInt gid,
                            BndJacobianFunc * g0,
                            BndJacobianFunc * g1,
                            BndJacobianFunc * g2,
                            BndJacobianFunc * g3);

    virtual void add_boundary() override;

    /// WeakForm object
    WeakForm * wf;

    /// Boundary number
    PetscInt bd;

public:
    static Parameters parameters();
};

} // namespace godzilla
