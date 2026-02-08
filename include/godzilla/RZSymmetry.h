// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Types.h"
#include "godzilla/DenseVector.h"

namespace godzilla {

class DiscreteProblemInterface;

class RZSymmetry : public Object {
public:
    RZSymmetry(const Parameters & pars);

    void create() override;
    Real get_value(Real time, const DenseVector<Real, 2> & x);

private:
    /// Discrete problem this object is part of
    Ref<DiscreteProblemInterface> dpi;
    /// Axis vector
    const std::vector<Real> axis;
    /// Axis point
    const std::vector<Real> pt;

public:
    static Parameters parameters();
};

} // namespace godzilla
