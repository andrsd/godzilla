// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Functional.h"

namespace godzilla {

class FEProblemInterface;

class ResidualFunc : public Functional {
public:
    ResidualFunc(FEProblemInterface * fepi, const std::string & region = "");

    /// Evaluate this functional
    ///
    /// @param val Array to store the values into
    virtual void evaluate(Scalar val[]) const = 0;
};

} // namespace godzilla
