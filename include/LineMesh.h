#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 1D line
///
class LineMesh : public UnstructuredMesh {
public:
    LineMesh(const InputParameters & parameters);

    /// Get the lower bound in x-direction
    ///
    /// @return Lower bound in x-direction
    PetscReal getXMin();

    /// Get the upper bound in x-direction
    ///
    /// @return Upper bound in x-direction
    PetscReal getXMax();

    /// Get the number of divisions in the x-direction
    ///
    /// @return Number of divisions in the x-direction
    PetscInt getNx();

protected:
    virtual void createDM() override;

    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmax;
    /// Number of grid point in the x direction
    const PetscInt & nx;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static InputParameters validParams();
};

} // namespace godzilla
