#pragma once

#include "godzilla/GodzillaConfig.h"
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
    NO_DISCARD const Normal & get_normal() const;
};

} // namespace godzilla