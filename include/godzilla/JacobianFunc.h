// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Functional.h"
#include "godzilla/FieldValue.h"
#include "godzilla/String.h"

namespace godzilla {

class FEProblemInterface;

class JacobianFunc : public Functional {
public:
    explicit JacobianFunc(FEProblemInterface * fepi, String region = "");

    /// Evaluate this functional
    ///
    /// @param val Array to store the values into
    virtual void evaluate(Scalar val[]) const = 0;

protected:
    /// Get the multiplier a for dF/dU_t
    ///
    /// @return The multiplier a for dF/dU_t
    const Real & get_time_shift() const;
};

} // namespace godzilla
