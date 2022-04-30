#pragma once

#include "Mesh.h"

namespace godzilla {

/// 1D line
///
class LineMesh : public Mesh {
public:
    LineMesh(const InputParameters & parameters);

    /// Get the lower bound in x-direction
    ///
    /// @return Lower bound in x-direction
    Real get_xmin();

    /// Get the upper bound in x-direction
    ///
    /// @return Upper bound in x-direction
    Real get_xmax();

    /// Get the number of divisions in the x-direction
    ///
    /// @return Number of divisions in the x-direction
    uint get_nx();

protected:
    virtual void create_dm() override;

    /// Minimum in the x direction
    const Real & xmin;
    /// Maximum in the x direction
    const Real & xmax;
    /// Number of grid point in the x direction
    const uint & nx;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
