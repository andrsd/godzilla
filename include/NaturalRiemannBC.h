#pragma once

#include "BoundaryCondition.h"
#include "Types.h"

namespace godzilla {

/// Base class for natural Riemann boundary conditions
class NaturalRiemannBC : public BoundaryCondition {
public:
    explicit NaturalRiemannBC(const Parameters & params);

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
    void add_boundary() override;

    /// Boundary number
    PetscInt bd;

public:
    static Parameters parameters();
};

} // namespace godzilla
