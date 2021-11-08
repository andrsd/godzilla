#pragma once

#include "BoundaryCondition.h"

namespace godzilla {

/// x^2 + y^2 boundary conditions
///
class X2Y2BC : public BoundaryCondition
{
public:
    X2Y2BC(const InputParameters & params);

    virtual PetscInt getNumComponents() const;
    virtual std::vector<DMBoundaryConditionType> getBcType() const;

protected:
    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]);

public:
    static InputParameters validParams();
};

} // godzilla
