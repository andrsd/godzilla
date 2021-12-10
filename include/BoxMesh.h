#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 3D box mesh
///
class BoxMesh : public UnstructuredMesh {
public:
    /// Constructor for building the object via Factory
    BoxMesh(const InputParameters & parameters);

    /// Get lower limit in x-direction
    PetscInt getXMin() const;

    /// Get upper limit in x-direction
    PetscInt getXMax() const;

    /// Get the number of grid points in x direction
    PetscInt getNx() const;

    /// Get lower limit in y-direction
    PetscInt getYMin() const;

    /// Get upper limit in y-direction
    PetscInt getYMax() const;

    /// Get the number of grid points in y-direction
    PetscInt getNy() const;

    /// Get lower limit in z-direction
    PetscInt getZMin() const;

    /// Get upper limit in z-direction
    PetscInt getZMax() const;

    /// Get the number of grid points in z direction
    PetscInt getNz() const;

protected:
    virtual void createDM() override;

    /// Minimum in the x direction
    const PetscReal & xmin;

    /// Maximum in the x direction
    const PetscReal & xmax;

    /// Minimum in the y direction
    const PetscReal & ymin;

    /// Maximum in the y direction
    const PetscReal & ymax;

    /// Minimum in the z direction
    const PetscReal & zmin;

    /// Maximum in the z direction
    const PetscReal & zmax;

    /// Number of grid point in the x direction
    const PetscInt & nx;

    /// Number of grid point in the y direction
    const PetscInt & ny;

    /// Number of grid point in the z direction
    const PetscInt & nz;

    /// True for simplices, False for tensor cells
    PetscBool simplex;

    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    /// Method for building InputParameters for this class
    static InputParameters validParams();
};

} // namespace godzilla
