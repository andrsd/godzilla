#pragma once

#include "GodzillaConfig.h"
#include "InitialCondition.h"
#include <vector>

namespace godzilla {

/// Constant initial condition
///
/// Use to set the initial condition of a field to constant values
/// Note that if the field has multiple components, you can provide a single
/// value for each component
class ConstantInitialCondition : public InitialCondition {
public:
    explicit ConstantInitialCondition(const Parameters & params);

    NO_DISCARD Int get_num_components() const override;

    void evaluate(Int dim, Real time, const Real x[], Int Nc, Scalar u[]) override;

protected:
    /// Constant values -- one for each component
    const std::vector<Real> & values;

public:
    static Parameters parameters();
};

} // namespace godzilla
