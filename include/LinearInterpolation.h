#pragma once

#include <vector>
#include "petsc.h"

namespace godzilla {

/// Utility class for linear interpolation
///
class LinearInterpolation {
public:
    /// Construct an empty linear interpolation object
    LinearInterpolation();
    /// Construct interpolation object by providing independent and dependent values
    ///
    /// @param x Independent values
    /// @param y Dependent values
    LinearInterpolation(const std::vector<PetscReal> & x, const std::vector<PetscReal> & y);

    /// Create the interpolation object by providing independent and dependent values
    ///
    /// @param x Independent values
    /// @param y Dependent values
    void create(const std::vector<PetscReal> & x, const std::vector<PetscReal> & y);

    /// Sample the interpolation at a point
    ///
    /// @params x Point where we sample the interpolation
    /// @return Interpolated value
    PetscReal sample(PetscReal x);

protected:
    /// Check that the supplied values are consistent
    void check();

    /// Independent values
    std::vector<PetscReal> x;
    /// Dependent values
    std::vector<PetscReal> y;
};

} // namespace godzilla
