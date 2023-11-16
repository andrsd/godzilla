#pragma once

#include "godzilla/GodzillaConfig.h"
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
    NO_DISCARD const Normal & get_normal() const;
};

} // namespace godzilla
