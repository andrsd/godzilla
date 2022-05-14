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

    /// Store the mesh
    ///
    /// @param dm DM holding the mesh to store into the file
    virtual void output_mesh(DM dm) = 0;

    /// Store the solution vector
    ///
    /// @param vec Solution vector to store into the file
    virtual void output_solution(Vec vec) = 0;

    /// Output a step of a multi-step simulation
    ///
    /// @param stepi Index of a step, -1 indicates no sequence
    /// @param dm DM with the mesh
    /// @param vec Solution vector
    virtual void output_step(PetscInt stepi, DM dm, Vec vec) = 0;

protected:
    /// Problem to get data from
    const Problem & problem;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
