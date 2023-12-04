// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Postprocessor.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

/// Compute the L_2 difference between a function 'u' and an FEM interpolant solution 'u_h'
///
class L2Diff : public Postprocessor, public FunctionInterface {
public:
    explicit L2Diff(const Parameters & params);

    void create() override;
    void compute() override;
    Real get_value() override;

    /// Evaluate the function 'u'
    ///
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param Nc The number of components
    /// @param u  The output field values
    void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]);

private:
    /// Computed L_2 error
    Real l2_diff;

public:
    static Parameters parameters();
};

} // namespace godzilla
