#pragma once

#include "BoundaryCondition.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
class DirichletBC : public BoundaryCondition, public FunctionInterface {
public:
    DirichletBC(const InputParameters & params);

    virtual PetscInt getNumComponents() const;
    virtual std::vector<DMBoundaryConditionType> getBcType() const;

protected:
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]);

public:
    static InputParameters validParams();
};

} // namespace godzilla
