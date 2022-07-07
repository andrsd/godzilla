#pragma once

#include "BoundaryCondition.h"
#include "Types.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    EssentialBC(const Parameters & params);

    virtual DMBoundaryConditionType get_bc_type() const override;

    /// Evaluate the boundary condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]) = 0;

    /// Evaluate time derivative of the boundary condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate_t(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscScalar u[]) = 0;

    /// Get pointer to the C function that will be passed into PETSc API
    virtual PetscFunc * get_function();

    /// Get pointer to the C function that will be passed into PETSc API for time derivatives
    virtual PetscFunc * get_function_t();

    /// Get the pointer to the context that will be passed into PETSc API
    virtual void * get_context();

public:
    static Parameters valid_params();
};

} // namespace godzilla
