#pragma once

#include "BoundaryCondition.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    EssentialBC(const InputParameters & params);

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

public:
    static InputParameters valid_params();
};

PetscErrorCode __essential_boundary_condition_function(PetscInt dim,
                                                       PetscReal time,
                                                       const PetscReal x[],
                                                       PetscInt nc,
                                                       PetscScalar u[],
                                                       void * ctx);
PetscErrorCode __essential_boundary_condition_function_t(PetscInt dim,
                                                         PetscReal time,
                                                         const PetscReal x[],
                                                         PetscInt nc,
                                                         PetscScalar u[],
                                                         void * ctx);

} // namespace godzilla
