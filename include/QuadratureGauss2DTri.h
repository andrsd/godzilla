#pragma once

#include "Quadrature2D.h"

namespace godzilla {

/// Gauss quadrature for 2D triangle
///
class QuadratureGauss2DTri : public Quadrature2D {
public:
    QuadratureGauss2DTri();
    virtual ~QuadratureGauss2DTri();

    const static int MAX_ORDER = 20;

    /// Get instance of this Gauss quadrature
    static QuadratureGauss2DTri & get();
};

} // namespace godzilla
