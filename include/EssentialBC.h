#pragma once

#include "BoundaryCondition.h"
#include "Types.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    explicit EssentialBC(const Parameters & params);

    void create() override;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    NO_DISCARD virtual Int get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    NO_DISCARD virtual const std::vector<Int> & get_components() const = 0;

    /// Evaluate the boundary condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;

    /// Evaluate time derivative of the boundary condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void evaluate_t(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;

    /// Get pointer to the C function that will be passed into PETSc API
    virtual PetscFunc * get_function();

    /// Get pointer to the C function that will be passed into PETSc API for time derivatives
    virtual PetscFunc * get_function_t();

    /// Get the pointer to the context that will be passed into PETSc API
    virtual void * get_context();

    void set_up() override;

protected:
    /// Field ID this boundary condition is attached to
    Int fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
