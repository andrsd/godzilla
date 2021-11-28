#pragma once

#include "BoundaryCondition.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    EssentialBC(const InputParameters & params);

    virtual DMBoundaryConditionType getBcType() const override;

    /// Evaluate the boundary condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

protected:
    virtual void setUpCallback() override;

public:
    static InputParameters validParams();
};

} // namespace godzilla
