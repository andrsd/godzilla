#pragma once

#include "GodzillaConfig.h"
#include "ResidualFunc.h"
#include "FieldValue.h"
#include <string>
#include "petsc.h"

namespace godzilla {

class NaturalBC;
class FEProblemInterface;

class BndResidualFunc : public ResidualFunc {
public:
    explicit BndResidualFunc(const NaturalBC * nbc);

protected:
    /// Get normal
    ///
    /// @return Outward normal
    NO_DISCARD const Normal & get_normal() const;
};

} // namespace godzilla
