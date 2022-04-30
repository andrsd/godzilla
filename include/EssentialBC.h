#pragma once

#include "BoundaryCondition.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    EssentialBC(const InputParameters & params);

    virtual BoundaryConditionType get_bc_type() const override;

    /// Evaluate the boundary condition
    ///
    virtual Scalar evaluate(Real x, Real y, Real z) const = 0;

public:
    static InputParameters validParams();
};

} // namespace godzilla
