#pragma once

#include "godzilla/UnstructuredMesh.h"

namespace godzilla {

/// 2D rectangular mesh
///
class RectangleMesh : public UnstructuredMesh {
public:
    explicit RectangleMesh(const Parameters & parameters);

    void create() override;
    ///
    NO_DISCARD Real get_x_min() const;
    NO_DISCARD Real get_x_max() const;
    /// Get the number of mesh points in x direction
    NO_DISCARD Int get_nx() const;
    ///
    NO_DISCARD Real get_y_min() const;
    NO_DISCARD Real get_y_max() const;
    /// Get the number of mesh points in y direction
    NO_DISCARD Int get_ny() const;

protected:
    DM create_dm() override;

    /// Minimum in the x direction
    const Real & xmin;
    /// Maximum in the x direction
    const Real & xmax;
    /// Minimum in the y direction
    const Real & ymin;
    /// Maximum in the y direction
    const Real & ymax;
    /// Number of mesh point in the x direction
    const Int & nx;
    /// Number of mesh point in the y direction
    const Int & ny;
    /// True for simplices, False for tensor cells
    PetscBool simplex;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
