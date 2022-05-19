#pragma once

#include "Object.h"
#include "PrintInterface.h"

namespace godzilla {

class FEProblemInterface;

/// Base class for initial conditions
///
class InitialCondition : public Object, public PrintInterface {
public:
    InitialCondition(const InputParameters & params);

    virtual void create();
    virtual PetscInt get_field_id() const;
    virtual PetscInt get_num_components() const = 0;

protected:
    /// Evaluate the initial condition
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[]) = 0;

    /// FE problem this object is part of
    const FEProblemInterface * fepi;

    /// Field ID this initial condition is attached to
    PetscInt fid;

public:
    static InputParameters valid_params();

    friend PetscErrorCode __initial_condition_function(PetscInt dim,
                                                       PetscReal time,
                                                       const PetscReal x[],
                                                       PetscInt Nc,
                                                       PetscScalar u[],
                                                       void * ctx);
};

PetscErrorCode __initial_condition_function(PetscInt dim,
                                            PetscReal time,
                                            const PetscReal x[],
                                            PetscInt Nc,
                                            PetscScalar u[],
                                            void * ctx);

} // namespace godzilla
