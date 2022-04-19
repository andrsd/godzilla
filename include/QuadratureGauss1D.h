#pragma once

#include "Quadrature1D.h"

namespace godzilla {

/// Gauss quadrature for 1D
///
class QuadratureGauss1D : public Quadrature1D {
public:
    QuadratureGauss1D();

    static const uint MAX_ORDER = 24;

    /// Get instance of the 1D Gauss quadrature
    static QuadratureGauss1D & get();
};

} // namespace godzilla
