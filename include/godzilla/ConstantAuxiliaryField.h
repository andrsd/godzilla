// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/AuxiliaryField.h"

namespace godzilla {

/// Constant auxiliary field
///
class ConstantAuxiliaryField : public AuxiliaryField {
public:
    explicit ConstantAuxiliaryField(const Parameters & params);

    [[nodiscard]] Int get_num_components() const override;
    void evaluate(Real time, const Real x[], Scalar u[]) override;

private:
    /// Values (one per component)
    const std::vector<Real> & values;

public:
    static Parameters parameters();
};

} // namespace godzilla
