#pragma once

#include "Quadrature1D.h"

namespace godzilla {

class QuadratureOutput1D : public Quadrature1D {
public:
    QuadratureOutput1D();

public:
    /// Get instance of the 1D output quadrature
    static QuadratureOutput1D & get();
};

} // namespace godzilla
