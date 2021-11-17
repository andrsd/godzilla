#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 3D box mesh
///
class BoxMesh : public UnstructuredMesh {
public:
    BoxMesh(const InputParameters & parameters);

    ///
    PetscInt getXMin() const;
    PetscInt getXMax() const;
    /// Get the number of grid points in x direction
    PetscInt getNx() const;
    ///
    PetscInt getYMin() const;
    PetscInt getYMax() const;
    /// Get the number of grid points in y direction
    PetscInt getNy() const;
    ///
    PetscInt getZMin() const;
    PetscInt getZMax() const;
    /// Get the number of grid points in z direction
    PetscInt getNz() const;

    virtual void create();

protected:
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
    static InputParameters validParams();
};

} // namespace godzilla
