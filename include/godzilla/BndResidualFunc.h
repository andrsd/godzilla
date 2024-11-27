// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/ResidualFunc.h"
#include "godzilla/FieldValue.h"
#include <string>

namespace godzilla {

class NaturalBC;
class FEProblemInterface;

class BndResidualFunc : public ResidualFunc {
public:
    explicit BndResidualFunc(const BoundaryCondition * bc);

protected:
    /// Get normal
    ///
    /// @return Outward normal
    const Normal & get_normal() const;
};

} // namespace godzilla
