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
    explicit NaturalBC(const Parameters & params);

    void create() override;
    void set_up() override;

    /// Get the label this boundary condition operates on
    ///
    /// @return The label
    NO_DISCARD virtual DMLabel get_label() const;

    /// Get the label values this boundary condition operates on
    ///
    /// @return The label values
    NO_DISCARD virtual const std::vector<Int> & get_ids() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    NO_DISCARD virtual Int get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    NO_DISCARD virtual const std::vector<Int> & get_components() const = 0;

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
    void set_jacobian_block(Int gid,
                            BndJacobianFunc * g0,
                            BndJacobianFunc * g1,
                            BndJacobianFunc * g2,
                            BndJacobianFunc * g3);

    /// DMLabel associated with the boundary name this boundary condition acts on
    DMLabel label;
    /// IDs of the label
    std::vector<Int> ids;
    /// Field ID this boundary condition is attached to
    Int fid;
    /// WeakForm object
    WeakForm * wf;

public:
    static Parameters parameters();
};

} // namespace godzilla
