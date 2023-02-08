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
    NO_DISCARD Real get_x_min() const;

    /// Get upper limit in x-direction
    NO_DISCARD Real get_x_max() const;

    /// Get the number of mesh points in x direction
    NO_DISCARD Int get_nx() const;

    /// Get lower limit in y-direction
    NO_DISCARD Real get_y_min() const;

    /// Get upper limit in y-direction
    NO_DISCARD Real get_y_max() const;

    /// Get the number of mesh points in y-direction
    NO_DISCARD Int get_ny() const;

    /// Get lower limit in z-direction
    NO_DISCARD Real get_z_min() const;

    /// Get upper limit in z-direction
    NO_DISCARD Real get_z_max() const;

    /// Get the number of mesh points in z direction
    NO_DISCARD Int get_nz() const;

protected:
    void create_dm() override;

    /// Minimum in the x direction
    const Real & xmin;

    /// Maximum in the x direction
    const Real & xmax;

    /// Minimum in the y direction
    const Real & ymin;

    /// Maximum in the y direction
    const Real & ymax;

    /// Minimum in the z direction
    const Real & zmin;

    /// Maximum in the z direction
    const Real & zmax;

    /// Number of mesh point in the x direction
    const Int & nx;

    /// Number of mesh point in the y direction
    const Int & ny;

    /// Number of mesh point in the z direction
    const Int & nz;

    /// True for simplices, False for tensor cells
    PetscBool simplex;

    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
