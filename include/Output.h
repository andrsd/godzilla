#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

class Problem;

/// Base class for doing output
///
class Output : public Object, public PrintInterface {
public:
    Output(const InputParameters & params);

    /// Output a step of a simulation
    ///
    /// @param stepi Index of a step, -1 indicates no sequence
    /// @param dm DM with the mesh
    /// @param vec Solution vector
    virtual void output_step(PetscInt stepi) = 0;

protected:
    /// Problem to get data from
    const Problem * problem;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
