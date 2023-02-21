#pragma once

#include "GodzillaConfig.h"
#include "Functional.h"
#include "FieldValue.h"
#include <string>
#include "petsc.h"

namespace godzilla {

class FEProblemInterface;

class JacobianFunc : public Functional {
public:
    explicit JacobianFunc(const FEProblemInterface * fepi, const std::string & region = "");

    /// Evaluate this functional
    ///
    /// @param val Array to store the values into
    virtual void evaluate(Scalar val[]) const = 0;

protected:
    /// Get physical coordinates
    ///
    /// @return Physical coordinates
    NO_DISCARD const Point & get_xyz() const;

    /// Get the multiplier a for dF/dU_t
    ///
    /// @return The multiplier a for dF/dU_t
    NO_DISCARD const Real & get_time_shift() const;
};

} // namespace godzilla
