#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// 1D line
///
class LineMesh : public UnstructuredMesh {
public:
    explicit LineMesh(const Parameters & parameters);

    /// Get the lower bound in x-direction
    ///
    /// @return Lower bound in x-direction
    NO_DISCARD PetscReal get_x_min() const;

    /// Get the upper bound in x-direction
    ///
    /// @return Upper bound in x-direction
    NO_DISCARD PetscReal get_x_max() const;

    /// Get the number of divisions in the x-direction
    ///
    /// @return Number of divisions in the x-direction
    NO_DISCARD PetscInt get_nx() const;

protected:
    void create_dm() override;

    /// Minimum in the x direction
    const PetscReal & xmin;
    /// Maximum in the x direction
    const PetscReal & xmax;
    /// Number of mesh point in the x direction
    const PetscInt & nx;
    /// create intermediate mesh pieces (edges, faces)
    PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
