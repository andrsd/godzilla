#pragma once

#include "GodzillaConfig.h"
#include "UnstructuredMesh.h"

namespace godzilla {

/// 3D box mesh
///
class BoxMesh : public UnstructuredMesh {
public:
    /// Constructor for building the object via Factory
    explicit BoxMesh(const Parameters & parameters);

    /// Get lower limit in x-direction
    NO_DISCARD PetscReal get_x_min() const;

    /// Get upper limit in x-direction
    NO_DISCARD PetscReal get_x_max() const;

    /// Get the number of mesh points in x direction
    NO_DISCARD PetscInt get_nx() const;

    /// Get lower limit in y-direction
    NO_DISCARD PetscReal get_y_min() const;

    /// Get upper limit in y-direction
    NO_DISCARD PetscReal get_y_max() const;

    /// Get the number of mesh points in y-direction
    NO_DISCARD PetscInt get_ny() const;

    /// Get lower limit in z-direction
    NO_DISCARD PetscReal get_z_min() const;

    /// Get upper limit in z-direction
    NO_DISCARD PetscReal get_z_max() const;

    /// Get the number of mesh points in z direction
    NO_DISCARD PetscInt get_nz() const;

protected:
    void create_dm() override;

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
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
