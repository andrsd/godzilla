#pragma once

#include "Functional.h"

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
