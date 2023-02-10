#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

class Functional {
public:
    explicit Functional(const FEProblemInterface * fepi);

    /// Evaluate this functional
    ///
    /// @param val Array to store the values into
    virtual void evaluate(Scalar val[]) const = 0;

protected:
    /// Get pointer to FEProblemInterface
    ///
    /// @return Pointer to FEProblemInterface
    NO_DISCARD const FEProblemInterface * get_fe_problem() const;

private:
    /// FEProblemInterface this functional is part of
    const FEProblemInterface * fepi;
};

} // namespace godzilla
