// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/InitialCondition.h"
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

    Int get_num_components() const override;

    void evaluate(Real time, const Real x[], Scalar u[]) override;

private:
    /// Constant values -- one for each component
    const std::vector<Real> values;

public:
    static Parameters parameters();
};

} // namespace godzilla
