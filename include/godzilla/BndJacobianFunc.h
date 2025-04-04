// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/JacobianFunc.h"
#include "godzilla/FieldValue.h"
#include <string>

namespace godzilla {

class FEProblemInterface;
class NaturalBC;

class BndJacobianFunc : public JacobianFunc {
public:
    explicit BndJacobianFunc(const BoundaryCondition * bc);

protected:
    /// Get normal
    ///
    /// @return Outward normal
    const Normal & get_normal() const;
};

} // namespace godzilla
