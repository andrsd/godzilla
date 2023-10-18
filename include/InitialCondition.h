#pragma once

#include "GodzillaConfig.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Types.h"
#include "DenseVector.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for initial conditions
///
class InitialCondition : public Object, public PrintInterface {
public:
    explicit InitialCondition(const Parameters & params);

    void create() override;
    virtual const std::string & get_field_name() const;
    NO_DISCARD virtual Int get_field_id() const;
    NO_DISCARD virtual Int get_num_components() const = 0;

    /// Get pointer to the C function that will be passed into PETSc API
    virtual PetscFunc * get_function();

    /// Get the pointer to the context that will be passed into PETSc API
    virtual void * get_context();

    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param nc The number of components
    /// @param u  The output field values
    virtual void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;

protected:
    /// Discrete problem this object is part of
    DiscreteProblemInterface * dpi;

    /// Field name this initial condition is attached to
    std::string field_name;

    /// Field ID this initial condition is attached to
    Int fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
