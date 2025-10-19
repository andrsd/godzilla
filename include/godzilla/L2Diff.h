// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Postprocessor.h"
#include "godzilla/Types.h"

namespace godzilla {

/// Compute the L_2 difference between a function 'u' and an FEM interpolant solution 'u_h'
///
class L2Diff : public Postprocessor {
public:
    explicit L2Diff(const Parameters & pars);

    void create() override;
    void compute() override;

    std::vector<Real> get_value() override;

    /// Evaluate the function 'u'
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    void evaluate(Real time, const Real x[], Scalar u[]);

private:
    ///
    FunctionDelegate delegate;
    /// Computed L_2 error
    Real l2_diff;

public:
    static Parameters parameters();
};

} // namespace godzilla
