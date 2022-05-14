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
    PetscInt get_x_min() const;

    /// Get upper limit in x-direction
    PetscInt get_x_max() const;

    /// Get the number of mesh points in x direction
    PetscInt get_nx() const;

    /// Get lower limit in y-direction
    PetscInt get_y_min() const;

    /// Get upper limit in y-direction
    PetscInt get_y_max() const;

    /// Get the number of mesh points in y-direction
    PetscInt get_ny() const;

    /// Get lower limit in z-direction
    PetscInt get_z_min() const;

    /// Get upper limit in z-direction
    PetscInt get_z_max() const;

    /// Get the number of mesh points in z direction
    PetscInt get_nz() const;

protected:
    virtual void create_dm() override;

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

    /// Number of mesh point in the x direction
    const PetscInt & nx;

    /// Number of mesh point in the y direction
    const PetscInt & ny;

    /// Number of mesh point in the z direction
    const PetscInt & nz;

    /// True for simplices, False for tensor cells
    PetscBool simplex;

    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    /// Method for building InputParameters for this class
    static InputParameters valid_params();
};

} // namespace godzilla
