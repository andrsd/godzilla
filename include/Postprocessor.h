
#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petsc.h"

namespace godzilla {

class Problem;

/// Base class for postprocessors
///
class Postprocessor : public Object, public PrintInterface {
public:
    Postprocessor(const InputParameters & params);
    virtual ~Postprocessor();

    /// Compute the postprocessor value
    ///
    virtual void compute() = 0;

    /// Get the computed value
    ///
    /// @return The value computed by the postprocessor
    virtual PetscReal get_value() = 0;

protected:
    /// Problem this object is part of
    const Problem * problem;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
