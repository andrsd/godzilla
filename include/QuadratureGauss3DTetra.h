#pragma once

#include "Quadrature3D.h"

namespace godzilla {

/// Gauss quadrature for 3D tetrahedron
///
class QuadratureGauss3DTetra : public Quadrature3D {
public:
    QuadratureGauss3DTetra();
    virtual ~QuadratureGauss3DTetra();

    const static int MAX_ORDER = 20;

    /// Get instance of this Gauss quadrature
    static QuadratureGauss3DTetra & get();
};

} // namespace godzilla
