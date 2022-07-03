#pragma once

#include "BoundaryCondition.h"
#include "Types.h"

namespace godzilla {

/// Base class for natural Riemann boundary conditions
class NaturalRiemannBC : public BoundaryCondition {
public:
    NaturalRiemannBC(const InputParameters & params);

    virtual DMBoundaryConditionType get_bc_type() const override;

    /// Evaluate the boundary condition
    ///
    /// @param time The time at which to sample
    /// @param c Centroid
    /// @param n Normal vector
    /// @param xI State at the interior cell
    /// @param xG State at the ghost cell (computed)
    virtual void evaluate(PetscReal time,
                          const PetscReal * c,
                          const PetscReal * n,
                          const PetscScalar * xI,
                          PetscScalar * xG) = 0;

protected:
    virtual void set_up_callback() override;

    /// WeakForm object
    PetscWeakForm wf;

    /// Boundary number
    PetscInt bd;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
